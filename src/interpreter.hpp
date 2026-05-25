#pragma once

#include <string>
#include "./ast.hpp"
#include "./error_service.hpp"

class Environment;

struct FunctionValue
{
  FunctionExpr *declaration;
};

struct Value
{
  enum Type
  {
    Number,
    String,
    Boolean,
    Nil,
    Undefined,
    Function,
  } type;

  int number{};

  std::string string{};

  bool boolean{};

  bool is_undefined{};

  FunctionValue function{};

  [[nodiscard]] std::string to_string() const {
    if (type == Number) {
      return std::to_string(number);
    }
    if (type == String) {
      return string;
    }
    if (type == Boolean) {
      return boolean ? "true" : "false";
    }
    if (type == Function) {
      return "<function>";
    }

    ErrorService::runtime_error("Cannot convert value of type to string", std::to_string(static_cast<int>(type)));
    return "";
  }

  [[nodiscard]] bool is_number() const { return type == Number; }

  [[nodiscard]] bool is_string() const { return type == String; }

  [[nodiscard]] bool is_boolean() const { return type == Boolean; }

  [[nodiscard]] bool is_nil() const { return type == Nil; }

  [[nodiscard]] bool is_function() const { return type == Function; }

  [[nodiscard]] bool equals(const Value &other) const {
    if (type != other.type) {
      return false;
    }

    switch (type) {
      case Number:
        return number == other.number;
      case String:
        return string == other.string;
      case Boolean:
        return boolean == other.boolean;
      case Nil:
        return true;
      case Undefined:
        break;
      case Function:
        return false;
    }

    return false;
  }

  [[nodiscard]] bool is_truthy() const {
    if (is_nil()) {
      return false;
    }
    if (is_boolean()) {
      return boolean;
    }
    if (is_number()) {
      return number != 0;
    }
    if (is_string()) {
      return !string.empty();
    }
    return true;
  }

  static Value number_value(const int n) { return Value{Number, n}; }

  static Value string_value(std::string s) { return Value{String, 0, std::move(s)}; }

  static Value boolean_value(const bool b) { return Value{Boolean, 0, "", b}; }

  static Value nil_value() { return Value{Nil, 0, ""}; }

  static Value undefined_value() { return Value{Undefined, 0, "", false, true}; }

  static Value function_value(FunctionExpr *declaration) { return Value{Function, 0, "", false, false, {declaration}}; }

  static std::string type_name(Type type) {
    switch (type) {
      case Number:
        return "Number";
      case String:
        return "String";
      case Boolean:
        return "Boolean";
      case Nil:
        return "Nil";
      case Undefined:
        return "Undefined";
      case Function:
        return "Function";
      default:
        return "Unknown";
    }
  }
};

struct ReturnSignal
{
  Value value;
};
