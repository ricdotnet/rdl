#include <iostream>
#include "./ast.hpp"
#include "./environment.hpp"
#include "./error_service.hpp"
#include "./runtime.hpp"

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
    result = Value::function_value(&expr);

    if (expr.receiver_type)
    {
      runtime->define_user_method(expr.name, expr);
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
    const auto condition = evaluate(stmt.condition.get());

    // need to check for truthiness of condition not just if it's a boolean
    if (condition.is_truthy())
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
    Environment local(env);
    Environment *previous = env;
    env = &local;

    while (evaluate(expr.condition.get()).is_truthy())
    {
      evaluate(expr.body.get());
    }

    env = previous;
    result = Value::nil_value();
  }

  void visit(RangeExpr &expr) override
  {
    result = Value::range_value(expr.start, expr.end, expr.step);
  }

  void visit(ForStmt &expr) override
  {
    Environment local(env);
    Environment *previous = env;
    env = &local;

    env->define(expr.iterator, Value::nil_value());

    // we have to normalize a mutable identifier
    const auto iterator = expr.iterator.substr(1);

    if (const auto iterable = evaluate(expr.iterable.get()); iterable.is_range())
    {
      const auto &[start, end, step] = iterable.range;

      try
      {
        for (int i = start; i < end; i += step)
        {
          env->assign(iterator, Value::number_value(i));
          evaluate(expr.body.get());
        }
      } catch (ReturnSignal &r)
      {
        env = previous;
        return;
      }
    }

    env = previous;
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
    env->assign(expr.name, value);

    result = value;
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
      default:
        ErrorService::runtime_error("Unknown binary operation", "\"" + expr.operation.value + "\"");
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
    std::vector<Value> arguments;

    for (auto &arg: expr.arguments)
    {
      arguments.push_back(evaluate(arg.get()));
    }

    if (runtime->builtins.contains(expr.function_name))
    {
      result = runtime->builtins.at(expr.function_name)(arguments);

      return;
    }

    const auto function = env->get(expr.function_name);

    if (!function.is_function())
    {
      ErrorService::runtime_error("Not a function", expr.function_name);
    }

    Environment local(env);
    Environment *previous = env;
    env = &local;

    const auto declaration = function.function.declaration;
    bind_local_params(local, *declaration, arguments);

    try
    {
      evaluate(declaration->body.get());
    } catch (ReturnSignal &r)
    {
      env = previous;
      result = r.value;
      return;
    }

    env = previous;
    result = Value::nil_value();
  }

  void visit(MethodCallExpr &expr) override
  {
    // The initial implementations for type methods do not need arguments
    const auto receiver = evaluate(expr.receiver.get());
    const auto &type_method = runtime->type_methods[receiver.type][expr.method_name];
    const auto &user_type_method = runtime->user_methods[Value::type_name(receiver.type)][expr.method_name];

    if (!type_method && !user_type_method)
    {
      ErrorService::runtime_error("Undefined method for type " + Value::type_name(receiver.type), expr.method_name);
      return;
    }

    Environment local(env);
    Environment *previous = env;
    env = &local;

    try
    {
      if (user_type_method)
      {
        env->define("self", receiver);
        result = evaluate(user_type_method->body.get());
      } else
      {
        result = type_method(receiver, std::vector<Value>());
      }
    } catch (ReturnSignal &r)
    {
      env = previous;
      result = r.value;
      return;
    }

    env = previous;
    result = Value::nil_value();
  }
};
