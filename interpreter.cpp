#include "ast.hpp"
#include "environment.hpp"

#include <iostream>

Value evaluate(Expr *expr, Environment &env) {
    if (auto num = dynamic_cast<NumberExpr *>(expr)) {
        return Value::number_value(num->value);
    }

    if (auto str = dynamic_cast<StringExpr *>(expr)) {
        return Value::string_value(str->value);
    }

    if (auto assign = dynamic_cast<AssignExpr *>(expr)) {
        Value value = evaluate(assign->value.get(), env);
        env.set(assign->name, value);

        return value;
    }

    if (auto variable = dynamic_cast<VariableExpr *>(expr)) {
        auto value = env.get(variable->name);
        if (value.is_undefined) {
            throw std::runtime_error("Undefined variable: " + variable->name);
        }
        return value;
    }

    if (auto bin = dynamic_cast<BinaryExpr *>(expr)) {
        Value left = evaluate(bin->left.get(), env);
        Value right = evaluate(bin->right.get(), env);

        if (left.type != Value::Number || right.type != Value::Number) {
            throw std::runtime_error("Binary operation requires number operands");
        }

        const int left_value = left.number;
        const int right_value = right.number;

        switch (bin->operation.type) {
            case TokenType::Plus:
                return Value::number_value(left_value + right_value);
            case TokenType::Minus:
                return Value::number_value(left_value - right_value);
            case TokenType::Star:
                return Value::number_value(left_value * right_value);
            case TokenType::Slash:
                return Value::number_value(left_value / right_value);

            default:
                throw std::runtime_error("Unknown operation");
        }
    }

    if (auto call = dynamic_cast<CallExpr *>(expr)) {
        if (call->function_name == "print") {
            for (auto &arg: call->arguments) {
                auto evaluated = evaluate(arg.get(), env);
                std::cout << evaluated.to_string();
            }
            std::cout << std::endl;

            return Value::nil_value();
        }

        throw std::runtime_error("Unknown function: " + call->function_name);
    }

    throw std::runtime_error("Unknown expression");
}
