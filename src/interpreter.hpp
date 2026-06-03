#pragma once

#include <format>
#include <functional>
#include <memory>
#include <string>
#include "./error_service.hpp"
#include "./value_type.hpp"

// Forward declarations
struct Value;

class Environment;

class FunctionExpr;

struct FunctionValue
{
  FunctionExpr *declaration = nullptr;

  std::function<Value(std::vector<Value> &)> builtin;

  bool is_builtin = false;
};

struct RangeValue
{
  int start;

  int end;

  int step;

  bool inclusive;
};

struct ObjectValue
{
  std::shared_ptr<std::unordered_map<std::string, Value> > properties;
};

struct ArrayValue
{
  ValueType type;

  std::shared_ptr<std::vector<Value> > elements;
};

struct Value
{
  ValueType type;

  int number{};

  std::string string{};

  bool boolean{};

  ObjectValue object{};

  bool is_undefined{};

  FunctionValue function{};

  RangeValue range{};

  ArrayValue array{};

  [[nodiscard]] std::string to_string() const
  {
    if (type == ValueType::Number)
    {
      return std::to_string(number);
    }
    if (type == ValueType::String)
    {
      return string;
    }
    if (type == ValueType::Boolean)
    {
      return boolean ? "true" : "false";
    }
    if (type == ValueType::Object)
    {
      return "<object>";
    }
    if (type == ValueType::Nil)
    {
      return "nil";
    }
    if (type == ValueType::Undefined)
    {
      return "undefined";
    }
    if (type == ValueType::Function)
    {
      return "<function>";
    }
    if (type == ValueType::Range)
    {
      return std::format("range({},{},{})", range.start, range.end, range.step);
    }
    if (type == ValueType::Array)
    {
      return std::format("<array:{}>", array.elements->size());
    }

    ErrorService::runtime_error("Cannot convert value of type to string", std::to_string(static_cast<int>(type)));
    return "";
  }

  [[nodiscard]] bool is_number() const { return type == ValueType::Number; }

  [[nodiscard]] bool is_string() const { return type == ValueType::String; }

  [[nodiscard]] bool is_boolean() const { return type == ValueType::Boolean; }

  [[nodiscard]] bool is_nil() const { return type == ValueType::Nil; }

  [[nodiscard]] bool is_function() const { return type == ValueType::Function; }

  [[nodiscard]] bool is_range() const { return type == ValueType::Range; }

  [[nodiscard]] bool is_object() const { return type == ValueType::Object; }

  [[nodiscard]] bool is_array() const { return type == ValueType::Array; }

  [[nodiscard]] bool equals(const Value &other) const
  {
    if (type != other.type)
    {
      return false;
    }

    switch (type)
    {
      case ValueType::Number:
        return number == other.number;
      case ValueType::String:
        return string == other.string;
      case ValueType::Boolean:
        return boolean == other.boolean;
      case ValueType::Nil:
        return true;
      // an object can never be equal to another one
      // but because we will share references for the same object then they will be equal
      // TODO: implement object equality check
      case ValueType::Object:
      case ValueType::Undefined:
      case ValueType::Function:
      case ValueType::Range:
      case ValueType::Array:
        break;
    }

    return false;
  }

  [[nodiscard]] bool is_truthy() const
  {
    if (is_nil())
    {
      return false;
    }
    if (is_boolean())
    {
      return boolean;
    }
    if (is_number())
    {
      return number != 0;
    }
    if (is_string())
    {
      return !string.empty();
    }
    if (is_array())
    {
      return !array.elements->empty();
    }
    return true;
  }

  static Value nil_value() { return Value{ValueType::Nil}; }

  static Value number_value(const int n) { return Value{ValueType::Number, n}; }

  static Value string_value(std::string s) { return Value{ValueType::String, 0, std::move(s)}; }

  static Value boolean_value(const bool b) { return Value{ValueType::Boolean, 0, "", b}; }

  static Value object_value(const std::shared_ptr<std::unordered_map<std::string, Value> > &properties)
  {
    return Value{ValueType::Object, 0, "", false, {properties},};
  }

  static Value undefined_value() { return Value{ValueType::Undefined, 0, "", false, {}, true}; }

  static Value builtin_function_value(std::function<Value(std::vector<Value> &)> body)
  {
    return Value{ValueType::Function, 0, "", false, {}, false, {nullptr, body, true}};
  }

  static Value user_function_value(FunctionExpr *declaration)
  {
    return Value{ValueType::Function, 0, "", false, {}, false, {declaration, nullptr, false}};
  }

  static Value range_value(const int start, const int end, const int step, const bool inclusive)
  {
    return Value{ValueType::Range, 0, "", false, {}, false, {nullptr, nullptr, false}, {start, end, step, inclusive}};
  }

  static Value array_value(const ValueType type, const std::shared_ptr<std::vector<Value> > &elements)
  {
    return Value{
      ValueType::Array, 0, "", false, {}, false, {nullptr, nullptr, false}, {0, 0, 0, false}, {type, elements}
    };
  }

  static std::string type_name(const ValueType type)
  {
    switch (type)
    {
      case ValueType::Number:
        return "Number";
      case ValueType::String:
        return "String";
      case ValueType::Boolean:
        return "Boolean";
      case ValueType::Nil:
        return "Nil";
      case ValueType::Undefined:
        return "Undefined";
      case ValueType::Function:
        return "Function";
      case ValueType::Range:
        return "Range";
      case ValueType::Object:
        return "Object";
      case ValueType::Array:
        return "Array";
      default:
        return "Unknown";
    }
  }

  static ValueType type_of(const std::optional<std::string> &value)
  {
    if (!value.has_value()) return ValueType::Nil;
    if (*value == "Number")
    {
      return ValueType::Number;
    }
    if (*value == "String")
    {
      return ValueType::String;
    }
    if (*value == "Boolean")
    {
      return ValueType::Boolean;
    }
    if (*value == "Nil")
    {
      return ValueType::Nil;
    }
    if (*value == "Undefined")
    {
      return ValueType::Undefined;
    }
    if (*value == "Function")
    {
      return ValueType::Function;
    }
    if (*value == "Range")
    {
      return ValueType::Range;
    }
    if (*value == "Object")
    {
      return ValueType::Object;
    }
    if (*value == "Array")
    {
      return ValueType::Array;
    }

    ErrorService::runtime_error("Unknown type", *value);
    return ValueType::Nil;
  }
};

struct ReturnSignal
{
  Value value;
};
