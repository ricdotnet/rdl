#include "ast.hpp"
#include "environment.hpp"

#include <iostream>

#include "error_service.hpp"

class Interpreter : public ExprVisitor {
    Environment &env;

    static std::string concat_expr(const std::string &left, const std::string &right) {
        return left + right;
    }

public:
    Value result;

    explicit Interpreter(Environment &env) : env(env), result() {
    }

    Value evaluate(Expr *expr) {
        expr->accept(*this);
        return result;
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

        if (!left.is_number() || !right.is_number()) {
            ErrorService::runtime_error("Binary operation requires number operands",
                                        "Got " + token_type_to_string(static_cast<TokenType>(left.type)) + " and " +
                                        token_type_to_string(
                                            static_cast<TokenType>(right.type)));
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
            default:
                ErrorService::syntax_error("Unknown binary operation", expr.operation);
        }
    }

    void visit(ConcatExpr &expr) override {
        const auto left = evaluate(expr.left.get());
        const auto right = evaluate(expr.right.get());

        if ((!left.is_string() && !left.is_number()) || (!right.is_string() && !right.is_number())) {
            ErrorService::runtime_error("Concatenation requires string or number operands",
                                        "Got " + token_type_to_string(static_cast<TokenType>(left.type)) + " and " +
                                        token_type_to_string(
                                            static_cast<TokenType>(right.type)));
        }

        result = Value::string_value(left.to_string() + right.to_string());
    }

    void visit(CallExpr &expr) override {
        if (expr.function_name == "print") {
            for (auto &arg: expr.arguments) {
                auto evaluated = evaluate(arg.get());
                std::cout << evaluated.to_string();
            }
            std::cout << std::endl;

            result = Value::nil_value();
        } else {
            ErrorService::runtime_error("Unknown function", expr.function_name);
        }
    }
};
