#pragma once

#include <string>
#include "./ast.hpp"
#include "./error_service.hpp"

class Environment;

struct Value
{
  enum Type
  {
    Number,
    String,
    Boolean,
    Nil,
    Undefined
  } type;

  int number{};

  std::string string{};

  bool boolean{};

  bool is_undefined{};

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

    ErrorService::runtime_error("Cannot convert value of type to string", std::to_string(static_cast<int>(type)));
    return "";
  }

  [[nodiscard]] bool is_number() const { return type == Number; }

  [[nodiscard]] bool is_string() const { return type == String; }

  [[nodiscard]] bool is_boolean() const { return type == Boolean; }

  [[nodiscard]] bool is_nil() const { return type == Nil; }

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
    return true;
  }

  static Value number_value(const int n) { return Value{Number, n}; }

  static Value string_value(std::string s) { return Value{String, 0, std::move(s)}; }

  static Value boolean_value(const bool b) { return Value{Boolean, 0, "", b}; }

  static Value nil_value() { return Value{Nil, 0, ""}; }

  static Value undefined_value() { return Value{Undefined, 0, "", false, true}; }
};
