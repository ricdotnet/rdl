#include "./ast.hpp"
#include "./environment.hpp"
#include "./environment_guard.hpp"
#include "./error_service.hpp"
#include "./runtime.hpp"
#include "./native_modules/fs_module.hpp"
#include "./native_modules/http_module.hpp"
#include "./native_modules/io_module.hpp"
#include "./native_modules/time_module.hpp"
#include "./utils/string.hpp"

class Interpreter : public ExprVisitor
{
  RuntimeContext context;

  static std::string concat_expr(const std::string &left, const std::string &right)
  {
    return left + right;
  }

  static bool number_type(const Value &value1, const Value &value2)
  {
    return value1.is_number() && value2.is_number();
  }

  static void bind_local_params(Environment &local_env, const FunctionExpr &fn, const std::vector<Value> &args)
  {
    for (size_t i = 0; i < args.size(); i++)
    {
      local_env.define(fn.parameters[i], args[i]);
    }
  }

public:
  Value result;

  explicit Interpreter(const RuntimeContext context) : context(context), result() {}

  Value evaluate(Expr *expr)
  {
    expr->accept(*this);
    return result;
  }

  void execute_route(const BlockStmt &route, const Value &request, const Value &response)
  {
    Environment local(context.environment, context.runtime);
    EnvironmentGuard guard(context.environment, &local);

    // context.environment->define("request", request);
    context.environment->define("response", response);

    for (auto &expr: route.statements)
    {
      evaluate(expr.get());
    }
  }

  void visit(FunctionExpr &expr) override
  {
    result = Value::user_function_value(&expr);

    if (expr.receiver_type.has_value())
    {
      const auto &receiver_type = context.runtime->globals.at(expr.receiver_type.value());
      receiver_type.object.properties->insert({expr.name, result});
      return;
    }

    context.environment->define(expr.name, result);
  }

  void visit(ReturnStmt &stmt) override
  {
    auto value = Value::nil_value();

    if (stmt.value)
    {
      value = evaluate(stmt.value.get());
    }

    throw ReturnSignal{value};
  }

  void visit(IfStmt &stmt) override
  {
    // need to check for truthiness of condition not just if it's a boolean
    if (const auto condition = evaluate(stmt.condition.get()); condition.is_truthy())
    {
      evaluate(stmt.then_branch.get());
    } else if (stmt.else_branch)
    {
      evaluate(stmt.else_branch.get());
    }

    result = Value::nil_value();
  }

  void visit(BlockStmt &stmt) override
  {
    for (auto &expr: stmt.statements)
    {
      evaluate(expr.get());
    }

    result = Value::nil_value();
  }

  void visit(StructStmt &stmt) override
  {
    const auto type_name = stmt.name;
    std::unordered_map<std::string, ValueType> fields;
    for (const auto &[field_name, field_type]: stmt.fields)
    {
      fields[field_name] = field_type;
    }

    context.environment->define(type_name, Value::struct_value(type_name, fields));

    result = Value::nil_value();
  }

  void visit(WhileExpr &expr) override
  {
    while (evaluate(expr.condition.get()).is_truthy())
    {
      Environment local(context.environment, context.runtime);
      EnvironmentGuard guard(context.environment, &local);

      try
      {
        evaluate(expr.body.get());
      } catch (ReturnSignal &r)
      {
        result = r.value;
        return;
      }
    }

    result = Value::nil_value();
  }

  void visit(RangeExpr &expr) override
  {
    result = Value::range_value(expr.start, expr.end, expr.step, expr.inclusive);
  }

  void visit(ForStmt &expr) override
  {
    Environment local(context.environment, context.runtime);
    EnvironmentGuard guard(context.environment, &local);

    context.environment->define(expr.iterator, Value::nil_value());
    if (expr.index_name)
    {
      context.environment->define(*expr.index_name, Value::number_value(0));
    }

    // we have to normalize a mutable identifier
    const auto iterator = normalise_identifier(expr.iterator);
    const auto body = expr.body.get();
    const auto iterable = evaluate(expr.iterable.get());

    if (iterable.is_range() && !body->statements.empty())
    {
      const auto &[start, end, step, inclusive] = iterable.range;

      if (step <= 0)
      {
        ErrorService::runtime_error("Range step must be a positive number", "");
      }

      const int end_value = inclusive ? end + 1 : end;

      try
      {
        for (int i = start; i < end_value; i += step)
        {
          context.environment->assign(iterator, Value::number_value(i));
          evaluate(body);
        }
      } catch (ReturnSignal &)
      {}
    }

    if (iterable.is_array() && !body->statements.empty())
    {
      const auto &arr = iterable.array.elements;
      const auto size = static_cast<int>(arr->size());

      try
      {
        for (int i = 0; i < size; ++i)
        {
          context.environment->assign(iterator, arr->at(i));
          if (expr.index_name.has_value())
          {
            context.environment->assign(normalise_identifier(*expr.index_name), Value::number_value(i));
          }
          evaluate(body);
        }
      } catch (ReturnSignal &)
      {}
    }

    result = Value::nil_value();
  }

  void visit(NumberExpr &expr) override
  {
    result = Value::number_value(expr.value);
  }

  void visit(StringExpr &expr) override
  {
    result = Value::string_value(expr.value);
  }

  void visit(BooleanExpr &expr) override
  {
    result = Value::boolean_value(expr.value);
  }

  void visit(VariableExpr &expr) override
  {
    const auto value = context.environment->get(expr.name);

    if (value.is_undefined)
    {
      ErrorService::runtime_error("Undefined variable", expr.name);
    }

    result = value;
  }

  void visit(AssignExpr &expr) override
  {
    const auto value = evaluate(expr.value.get());

    if (const auto *left = dynamic_cast<VariableExpr *>(expr.left.get()))
    {
      context.environment->assign(left->name, value);
      result = value;
      return;
    }

    if (const auto *left = dynamic_cast<DotExpr *>(expr.left.get()))
    {
      const auto receiver = evaluate(left->receiver.get());

      if (receiver.type == ValueType::Struct)
      {
        auto &definition = receiver.struct_instance.definition;
        auto &fields = *receiver.struct_instance.fields;

        if (!definition->fields.contains(left->field_name))
        {
          ErrorService::runtime_error("Undefined field for struct " + definition->name, left->field_name);
        }

        if (definition->fields.at(left->field_name) != value.type)
        {
          ErrorService::runtime_error(
            "Type mismatch for struct field assignment: expected " +
            Value::type_name(definition->fields.at(left->field_name)) + ", got " + Value::type_name(value.type),
            left->field_name);
        }

        fields[left->field_name] = value;
        result = value;
        return;
      }

      auto &prop_map = *receiver.object.properties;
      prop_map[left->field_name] = value;
      result = value;
      return;
    }

    if (const auto *left = dynamic_cast<IndexExpr *>(expr.left.get()))
    {
      const auto receiver = evaluate(left->receiver_array.get());
      const auto indexVal = evaluate(left->index.get());

      if (receiver.array.type != value.type)
      {
        ErrorService::runtime_error(
          "Type mismatch in array element: expected " + Value::type_name(receiver.array.type) + ", got " +
          Value::type_name(value.type), "Array elements must match the declared type");
      }

      const auto &arr = receiver.array.elements;

      receiver.array.elements->insert(arr->begin() + static_cast<int>(indexVal.number), value);
      result = value;
      return;
    }

    ErrorService::runtime_error("Invalid assignment target", "");
  }

  void visit(LetExpr &expr) override
  {
    const auto value = evaluate(expr.initialiser.get());

    context.environment->define(expr.name, value);

    result = Value::nil_value();
  }

  void visit(BinaryExpr &expr) override
  {
    const auto left = evaluate(expr.left.get());
    const auto right = evaluate(expr.right.get());

    switch (expr.operation.type)
    {
      case TokenType::Plus:
      case TokenType::Minus:
      case TokenType::Star:
      case TokenType::Slash:
      case TokenType::Greater:
      case TokenType::GreaterEqual:
      case TokenType::Less:
      case TokenType::LessEqual: {
        if (!number_type(left, right))
        {
          ErrorService::runtime_error("Expected numbers ",
                                      "Found " + left.to_string() + " and " + right.to_string() + "");
        }

        const int left_value = static_cast<int>(left.number);
        const int right_value = static_cast<int>(right.number);

        switch (expr.operation.type)
        {
          case TokenType::Plus:
            result = Value::number_value(left_value + right_value);
            break;
          case TokenType::Minus:
            result = Value::number_value(left_value - right_value);
            break;
          case TokenType::Star:
            result = Value::number_value(left_value * right_value);
            break;
          case TokenType::Slash:
            result = Value::number_value(left_value / right_value);
            break;

          case TokenType::Greater:
            result = Value::boolean_value(left_value > right_value);
            break;
          case TokenType::GreaterEqual:
            result = Value::boolean_value(left_value >= right_value);
            break;
          case TokenType::Less:
            result = Value::boolean_value(left_value < right_value);
            break;
          case TokenType::LessEqual:
            result = Value::boolean_value(left_value <= right_value);
            break;

          default:
            break;
        }

        return;
      }
      case TokenType::EqualEqual:
        result = Value::boolean_value(left.equals(right));
        break;
      case TokenType::BangEqual:
        result = Value::boolean_value(!left.equals(right));
        break;
      case TokenType::And:
        result = Value::boolean_value(left.is_truthy() && right.is_truthy());
        break;
      case TokenType::Or:
        result = Value::boolean_value(left.is_truthy() || right.is_truthy());
        break;
      default:
        ErrorService::runtime_error("Unknown binary operation", "\"" + expr.operation.value + "\"");
    }
  }

  void visit(UnaryExpr &expr) override
  {
    result = Value::nil_value();

    const auto operation = expr.operation.type;
    const auto operand = evaluate(expr.operand.get());

    switch (operation)
    {
      case TokenType::Bang:
        result = Value::boolean_value(!operand.is_truthy());
        break;
      case TokenType::Minus: {
        if (!operand.is_number())
        {
          ErrorService::runtime_error("Expected number", operand.to_string());
        }

        const int value = static_cast<int>(operand.number);
        result = Value::number_value(-value);
        return;
      }

      default:
        ErrorService::runtime_error("Unknown unary operation", "\"" + expr.operation.value + "\"");
        break;
    }
  }

  void visit(ConcatExpr &expr) override
  {
    const auto left = evaluate(expr.left.get());
    const auto right = evaluate(expr.right.get());

    if ((!left.is_string() && !left.is_number() && !left.is_boolean()) || (
          !right.is_string() && !right.is_number() && !right.is_boolean()))
    {
      ErrorService::runtime_error("Concatenation requires string, number or boolean operands",
                                  "Found " + Value::type_name(left.type) + " and " + Value::type_name(right.type));
    }

    result = Value::string_value(left.to_string() + right.to_string());
  }

  void visit(CallExpr &expr) override
  {
    result = Value::nil_value();

    std::vector<Value> arguments;

    const auto callee = evaluate(expr.callee.get());

    if (!callee.is_function())
    {
      ErrorService::runtime_error("Not callable", callee.to_string());
    }

    arguments.reserve(expr.arguments.size());
    for (auto &arg: expr.arguments)
    {
      arguments.push_back(evaluate(arg.get()));
    }

    if (callee.function.kind == FunctionValue::Kind::Builtin)
    {
      result = callee.function.builtin(callee, arguments);
      return;
    }

    Environment local(context.environment, context.runtime);
    EnvironmentGuard guard(context.environment, &local);

    bind_local_params(local, *callee.function.declaration, arguments);

    try
    {
      evaluate(callee.function.declaration->body.get());
    } catch (ReturnSignal &r)
    {
      result = r.value;
    }
  }

  void visit(MethodCallExpr &expr) override
  {
    const auto receiver = evaluate(expr.receiver.get());
    std::unordered_map<std::string, Value> fields;

    if (receiver.type != ValueType::Object)
    {
      fields = *context.runtime->globals.at(Value::type_name(receiver.type)).object.properties;
    } else
    {
      fields = *receiver.object.properties;
      if (!fields.contains(expr.method_name))
      {
        ErrorService::runtime_error("Undefined property: " + expr.method_name, "");
        return;
      }
    }

    if (!fields.contains(expr.method_name))
    {
      ErrorService::runtime_error("Undefined method: " + expr.method_name, "");
      return;
    }
    const auto &method = fields.at(expr.method_name);

    if (!method.is_function())
    {
      ErrorService::runtime_error("Attempted to call non-function", "");
    }

    std::vector<Value> args;
    args.reserve(expr.arguments.size());

    for (auto &arg: expr.arguments)
    {
      args.push_back(evaluate(arg.get()));
    }

    if (method.function.kind == FunctionValue::Kind::Builtin)
    {
      result = method.function.builtin(receiver, args);
    } else
    {
      Environment local(context.environment, context.runtime);
      EnvironmentGuard guard(context.environment, &local);

      local.define("self", receiver);

      bind_local_params(local, *method.function.declaration, args);

      result = evaluate(method.function.declaration->body.get());
    }
  }

  void visit(ObjectExpr &expr) override
  {
    const auto internal_map = std::make_shared<std::unordered_map<std::string, Value> >();

    for (const auto &[field_name, field_expr]: expr.fields)
    {
      internal_map->insert({field_name, evaluate(field_expr.get())});
    }
    result = Value::object_value(internal_map);
  }

  void visit(DotExpr &expr) override
  {
    const auto field_name = expr.field_name;
    const auto receiver = evaluate(expr.receiver.get());

    if (receiver.type == ValueType::Struct)
    {
      const auto fields = *receiver.struct_instance.fields;
      const auto definition = receiver.struct_instance.definition;
      const auto struct_type = definition->name;

      if (fields.contains(field_name))
      {
        result = fields.at(field_name);
        return;
      }

      ErrorService::runtime_error("Undefined field for struct " + struct_type, field_name);
    }

    const auto prop_map = receiver.object.properties;

    if (prop_map->contains(field_name))
    {
      result = prop_map->at(field_name);
      return;
    }

    ErrorService::runtime_error("Undefined field for type " + Value::type_name(receiver.type), field_name);
  }

  void visit(ArrayExpr &expr) override
  {
    std::vector<Value> values;

    for (const auto &element_expr: expr.elements)
    {
      auto value = evaluate(element_expr.get());

      if (value.type != expr.declared_type)
      {
        ErrorService::runtime_error(
          "Type mismatch in array element: expected " + Value::type_name(expr.declared_type) + ", got " +
          Value::type_name(value.type), "Array elements must match the declared type");
      }

      values.push_back(value);
    }

    result = Value::array_value(expr.declared_type, std::make_shared<std::vector<Value> >(std::move(values)));
  }

  void visit(IndexExpr &expr) override
  {
    const Value receiver = evaluate(expr.receiver_array.get());
    const Value indexVal = evaluate(expr.index.get());

    if (!indexVal.is_number())
    {
      ErrorService::runtime_error("Array index must be a number", "");
    }

    if (!receiver.is_array())
    {
      ErrorService::runtime_error("Cannot index non-array", "");
    }

    const auto &arr = receiver.array.elements;
    const int size = static_cast<int>(arr->size());
    const int i = static_cast<int>(indexVal.number);

    if (i < 0 || i >= size)
    {
      ErrorService::runtime_error("Index out of bounds", "");
    }

    result = (*arr)[i];
  }

  void visit(StructInitExpr &expr) override
  {
    auto struct_definition = context.environment->get(expr.type_name).struct_definition;
    const auto &struct_fields = struct_definition.fields;

    const auto fields = std::make_shared<std::unordered_map<std::string, Value> >();
    fields->reserve(expr.fields.size());

    for (const auto &[field_name, field_expr]: expr.fields)
    {
      if (!struct_fields.contains(field_name))
      {
        ErrorService::runtime_error("Struct field '" + field_name + "' does not exist", "");
      }

      const auto &struct_field = struct_fields.at(field_name);
      const auto value = evaluate(field_expr.get());

      if (value.type != struct_field)
      {
        ErrorService::runtime_error(
          "Type mismatch in struct field '" + field_name + "': expected " + Value::type_name(struct_field) + ", got " +
          Value::type_name(value.type), "");
      }

      fields->insert({field_name, value});
    }

    result = Value::struct_instance_value(std::make_shared<StructDefinition>(struct_definition), fields);
  }

  void visit(ImportExpr &expr) override
  {
    const auto module_name = expr.module_name;
    Value module;

    if (module_name == "time")
    {
      module = std::make_shared<TimeModule>()->init();
    }
    if (module_name == "io")
    {
      module = std::make_shared<IoModule>(context)->init();
    }
    if (module_name == "fs")
    {
      module = std::make_shared<FileSystemModule>()->init();
    }
    if (module_name == "http")
    {
      module = std::make_shared<HttpModule>(context)->init();
    }

    context.runtime->add_global(module_name, module);

    result = Value::nil_value();
  }

  void visit(GroupStmt &stmt) override
  {
    for (const auto &route: stmt.routes)
    {
      route->accept(*this);
    }
  }

  void visit(RouteStmt &stmt) override
  {
    context.runtime->register_route(stmt.method, stmt.path, stmt.body.get());

    result = Value::nil_value();
  }
};
