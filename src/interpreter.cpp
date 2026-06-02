#include "./ast.hpp"
#include "./environment.hpp"
#include "./error_service.hpp"
#include "./runtime.hpp"
#include "environment_guard.hpp"

class Interpreter : public ExprVisitor
{
  Environment *env;

  Runtime *runtime;

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

  explicit Interpreter(Environment *env, Runtime *runtime) : env(env), runtime(runtime), result() {}

  Value evaluate(Expr *expr)
  {
    expr->accept(*this);
    return result;
  }

  void visit(FunctionExpr &expr) override
  {
    result = Value::user_function_value(&expr);

    if (expr.receiver_type)
    {
      runtime->define_user_method(Value::type_of(expr.receiver_type), expr.name, result);
      return;
    }

    env->define(expr.name, result);
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

  void visit(WhileExpr &expr) override
  {
    while (evaluate(expr.condition.get()).is_truthy())
    {
      Environment local(env);
      EnvironmentGuard guard(env, &local);

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
    result = Value::range_value(expr.start, expr.end, expr.step);
  }

  void visit(ForStmt &expr) override
  {
    Environment local(env);
    EnvironmentGuard guard(env, &local);

    env->define(expr.iterator, Value::nil_value());

    // we have to normalize a mutable identifier
    const auto iterator = expr.iterator.substr(1);
    const auto body = expr.body.get();

    if (const auto iterable = evaluate(expr.iterable.get()); iterable.is_range() && !body->statements.empty())
    {
      const auto &[start, end, step] = iterable.range;

      try
      {
        for (int i = start; i < end; i += step)
        {
          env->assign(iterator, Value::number_value(i));
          evaluate(body);
        }
      } catch (ReturnSignal &)
      {
        // ignore since there is no actual return value
      }
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
    const auto value = env->get(expr.name);

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
      env->assign(left->name, value);
      result = value;
      return;
    }

    if (const auto *left = dynamic_cast<DotExpr *>(expr.left.get()))
    {
      const auto receiver = evaluate(left->receiver.get());
      auto &prop_map = *receiver.object.properties;
      prop_map[left->field_name] = value;
      result = value;
      return;
    }

    ErrorService::runtime_error("Invalid assignment target", "");
  }

  void visit(LetExpr &expr) override
  {
    const auto value = evaluate(expr.initialiser.get());

    env->define(expr.name, value);

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

        const int left_value = left.number;
        const int right_value = right.number;

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

        const int value = operand.number;
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
      ErrorService::runtime_error("Not callable", "");
    }

    arguments.reserve(expr.arguments.size());
    for (auto &arg: expr.arguments)
    {
      arguments.push_back(evaluate(arg.get()));
    }

    if (callee.function.is_builtin)
    {
      result = callee.function.builtin(arguments);
      return;
    }

    Environment local(env);
    EnvironmentGuard guard(env, &local);

    bind_local_params(local, *callee.function.declaration, arguments);

    try
    {
      evaluate(callee.function.declaration->body.get());
    } catch (ReturnSignal &r)
    {
      result = r.value;
      return;
    }
  }

  void visit(MethodCallExpr &expr) override
  {
    result = Value::nil_value();

    // The initial implementations for type methods do not need arguments
    const auto receiver = evaluate(expr.receiver.get());
    const auto &type_method = runtime->type_methods[receiver.type][expr.method_name];
    const auto &user_type_method = runtime->user_methods[receiver.type][expr.method_name];

    if (!type_method && !user_type_method.is_function())
    {
      ErrorService::runtime_error("Undefined method for type " + Value::type_name(receiver.type), expr.method_name);
      return;
    }

    Environment local(env);
    EnvironmentGuard guard(env, &local);

    std::vector<Value> arguments;

    arguments.reserve(expr.arguments.size());
    for (auto &arg: expr.arguments)
    {
      arguments.push_back(evaluate(arg.get()));
    }

    try
    {
      if (user_type_method.is_function())
      {
        env->define("self", receiver);
        result = evaluate(user_type_method.function.declaration->body.get());
      } else
      {
        result = type_method(receiver, arguments);
      }
    } catch (ReturnSignal &r)
    {
      result = r.value;
      return;
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
    const int i = indexVal.number;

    if (i < 0 || i >= size)
    {
      ErrorService::runtime_error("Index out of bounds", "");
    }

    result = (*arr)[i];
  }
};
