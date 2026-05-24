#pragma once

#include "ast.hpp"

#include <string>
#include <stdexcept>

class Environment;

struct Value {
    enum Type { Number, String, Nil, Undefined } type;

    int number{};
    std::string string{};
    bool is_undefined{};

    [[nodiscard]] std::string to_string() const {
        if (type == Number) {
            return std::to_string(number);
        }
        if (type == String) {
            return string;
        }

        throw std::runtime_error(
            "Cannot convert value of type " + std::to_string(static_cast<int>(type)) + " to string");
    }

    static Value number_value(const int n) { return Value{Number, n}; }
    static Value string_value(std::string s) { return Value{String, 0, std::move(s)}; }
    static Value nil_value() { return Value{Nil, 0, ""}; }
    static Value undefined_value() { return Value{Undefined, 0, "", true}; }
};

Value evaluate(Expr *expr, Environment &env);
