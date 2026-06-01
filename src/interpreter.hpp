#pragma once

#include <format>
#include <functional>
#include <string>
#include "./ast.hpp"
#include "./error_service.hpp"

// Forward declaration
struct Value;

class Environment;

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
};

struct ObjectValue
{
  std::shared_ptr<std::unordered_map<std::string, Value> > properties;
};

struct Value
{
  enum Type
  {
    Number,
    String,
    Boolean,
    Object,
    Nil,
    Undefined,
    Function,
    Range,
  } type;

  int number{};

  std::string string{};

  bool boolean{};

  ObjectValue object{};

  bool is_undefined{};

  FunctionValue function{};

  RangeValue range{};

  [[nodiscard]] std::string to_string() const
  {
    if (type == Number)
    {
      return std::to_string(number);
    }
    if (type == String)
    {
      return string;
    }
    if (type == Boolean)
    {
      return boolean ? "true" : "false";
    }
    if (type == Object)
    {
      return "<object>";
    }
    if (type == Nil)
    {
      return "nil";
    }
    if (type == Undefined)
    {
      return "undefined";
    }
    if (type == Function)
    {
      return "<function>";
    }
    if (type == Range)
    {
      return std::format("range({},{},{})", range.start, range.end, range.step);
    }

    ErrorService::runtime_error("Cannot convert value of type to string", std::to_string(static_cast<int>(type)));
    return "";
  }

  [[nodiscard]] bool is_number() const { return type == Number; }

  [[nodiscard]] bool is_string() const { return type == String; }

  [[nodiscard]] bool is_boolean() const { return type == Boolean; }

  [[nodiscard]] bool is_nil() const { return type == Nil; }

  [[nodiscard]] bool is_function() const { return type == Function; }

  [[nodiscard]] bool is_range() const { return type == Range; }

  [[nodiscard]] bool is_object() const { return type == Object; }

  [[nodiscard]] bool equals(const Value &other) const
  {
    if (type != other.type)
    {
      return false;
    }

    switch (type)
    {
      case Number:
        return number == other.number;
      case String:
        return string == other.string;
      case Boolean:
        return boolean == other.boolean;
      case Nil:
        return true;
      // an object can never be equal to another one
      // but because we will share references for the same object then they will be equal
      // TODO: implement object equality check
      case Object:
      case Undefined:
      case Function:
      case Range:
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
    return true;
  }

  static Value number_value(const int n) { return Value{Number, n}; }

  static Value string_value(std::string s) { return Value{String, 0, std::move(s)}; }

  static Value boolean_value(const bool b) { return Value{Boolean, 0, "", b}; }

  static Value nil_value() { return Value{Nil, 0, ""}; }

  static Value undefined_value() { return Value{Undefined, 0, "", false, {}, true}; }

  static Value builtin_function_value(std::function<Value(std::vector<Value> &)> body)
  {
    return Value{Function, 0, "", false, {}, false, {nullptr, body, true}};
  }

  static Value user_function_value(FunctionExpr *declaration)
  {
    return Value{Function, 0, "", false, {}, false, {declaration, nullptr, false}};
  }

  static Value range_value(const int start, const int end, const int step)
  {
    return Value{Range, 0, "", false, {}, false, {nullptr, nullptr, false}, {start, end, step}};
  }

  static Value object_value(const std::shared_ptr<std::unordered_map<std::string, Value> > &properties)
  {
    return Value{Object, 0, "", false, {properties},};
  }

  static std::string type_name(const Type type)
  {
    switch (type)
    {
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
      case Range:
        return "Range";
      case Object:
        return "Object";
      default:
        return "Unknown";
    }
  }

  static Type type_of(const std::optional<std::string> &value)
  {
    if (!value.has_value()) return Nil;
    if (*value == "Number")
    {
      return Number;
    }
    if (*value == "String")
    {
      return String;
    }
    if (*value == "Boolean")
    {
      return Boolean;
    }
    if (*value == "Nil")
    {
      return Nil;
    }
    if (*value == "Undefined")
    {
      return Undefined;
    }
    if (*value == "Function")
    {
      return Function;
    }
    if (*value == "Range")
    {
      return Range;
    }
    if (*value == "Object")
    {
      return Object;
    }

    ErrorService::runtime_error("Unknown type", *value);
    return Nil;
  }
};

struct ReturnSignal
{
  Value value;
};
