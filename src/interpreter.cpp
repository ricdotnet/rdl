#include <iostream>
#include "./ast.hpp"
#include "./environment.hpp"
#include "./error_service.hpp"

class Interpreter : public ExprVisitor
{
  Environment &env;

  static std::string concat_expr(const std::string &left, const std::string &right) {
    return left + right;
  }

  static bool number_type(const Value &value1, const Value &value2) {
    return value1.is_number() && value2.is_number();
  }

public:
  Value result;

  explicit Interpreter(Environment &env) : env(env), result() {}

  Value evaluate(Expr *expr) {
    expr->accept(*this);
    return result;
  }

  void visit(FunctionExpr &expr) override {
    result = Value::function_value(&expr);

    env.define(expr.name, result);
  }

  void visit(IfStmt &stmt) override {
    const auto condition = evaluate(stmt.condition.get());

    // need to check for truthiness of condition not just if it's a boolean
    if (condition.is_truthy()) {
      evaluate(stmt.then_branch.get());
    } else if (stmt.else_branch) {
      evaluate(stmt.else_branch.get());
    }

    result = Value::nil_value();
  }

  void visit(BlockStmt &stmt) override {
    for (auto &expr: stmt.statements) {
      evaluate(expr.get());
    }
    result = Value::nil_value();
  }

  void visit(WhileExpr &expr) override {
    while (evaluate(expr.condition.get()).is_truthy()) {
      evaluate(expr.body.get());
    }

    result = Value::nil_value();
  }

  void visit(NumberExpr &expr) override {
    result = Value::number_value(expr.value);
  }

  void visit(StringExpr &expr) override {
    result = Value::string_value(expr.value);
  }

  void visit(VariableExpr &expr) override {
    const auto value = env.get(expr.name);
    if (value.is_undefined) {
      ErrorService::runtime_error("Undefined variable", expr.name);
    }
    result = value;
  }

  void visit(AssignExpr &expr) override {
    const auto value = evaluate(expr.value.get());
    env.assign(expr.name, value);

    result = value;
  }

  void visit(LetExpr &expr) override {
    const auto value = evaluate(expr.initialiser.get());

    env.define(expr.name, value);

    result = Value::nil_value();
  }

  void visit(BinaryExpr &expr) override {
    const auto left = evaluate(expr.left.get());
    const auto right = evaluate(expr.right.get());

    switch (expr.operation.type) {
      case TokenType::Plus:
      case TokenType::Minus:
      case TokenType::Star:
      case TokenType::Slash:
      case TokenType::Greater:
      case TokenType::GreaterEqual:
      case TokenType::Less:
      case TokenType::LessEqual: {
        if (!number_type(left, right)) {
          ErrorService::runtime_error("Expected numbers ",
                                      "Found " + left.to_string() + " and " + right.to_string() + "");
        }

        const int left_value = left.number;
        const int right_value = right.number;

        switch (expr.operation.type) {
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

  void visit(ConcatExpr &expr) override {
    const auto left = evaluate(expr.left.get());
    const auto right = evaluate(expr.right.get());

    if ((!left.is_string() && !left.is_number() && !left.is_boolean()) || (
          !right.is_string() && !right.is_number() && !right.is_boolean())) {
      ErrorService::runtime_error("Concatenation requires string, number or boolean operands",
                                  "Found " + Value::type_name(left.type) + " and " + Value::type_name(right.type));
    }

    result = Value::string_value(left.to_string() + right.to_string());
  }

  void visit(CallExpr &expr) override {
    std::vector<Value> arguments;

    for (auto &arg: expr.arguments) {
      arguments.push_back(evaluate(arg.get()));
    }

    if (env.builtins.contains(expr.function_name)) {
      result = env.builtins.at(expr.function_name)(arguments);

      return;
    }

    const auto function = env.get(expr.function_name);

    if (!function.is_function()) {
      ErrorService::runtime_error("Not a function", expr.function_name);
    }

    const auto declaration = function.function.declaration;

    for (size_t i = 0; i < declaration->parameters.size(); i++) {
      env.define(declaration->parameters[i], arguments[i]);
    }

    evaluate(declaration->body.get());

    result = Value::nil_value();
  }
};
