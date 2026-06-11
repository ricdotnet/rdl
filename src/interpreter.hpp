#pragma once

#include <format>
#include <fstream>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include "./error_service.hpp"
#include "./value_type.hpp"
#include "./libs/httplib.h"

// Forward declarations
struct Value;

class Environment;

class FunctionExpr;

using NativeFn = std::function<Value(const Value &receiver, std::vector<Value> &args)>;

struct NativeObject
{
  virtual ~NativeObject() = default;
};

struct FunctionValue
{
  enum class Kind
  {
    Builtin,
    UserDefined,
  };

  Kind kind;

  FunctionExpr *declaration = nullptr;

  NativeFn builtin;
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

  std::shared_ptr<NativeObject> native_object;
};

struct ArrayValue
{
  ValueType type;

  std::shared_ptr<std::vector<Value> > elements;
};

struct StructDefinition
{
  std::string name;

  std::unordered_map<std::string, ValueType> fields;
};

struct StructInstance
{
  std::shared_ptr<StructDefinition> definition;

  std::shared_ptr<std::unordered_map<std::string, Value> > fields;
};

struct RequestHandle : NativeObject
{
  httplib::Request *request = nullptr;
};

struct ResponseHandle : NativeObject
{
  httplib::Response *response = nullptr;
};

struct FileHandler : NativeObject
{
  std::ifstream stream;
};

struct Value
{
  ValueType type;

  ssize_t number{};

  std::string string{};

  bool boolean{};

  ObjectValue object{};

  bool is_undefined{};

  FunctionValue function{};

  RangeValue range{};

  ArrayValue array{};

  StructDefinition struct_definition{};

  StructInstance struct_instance{};

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
      case ValueType::Struct:
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

  static Value nil_value()
  {
    Value v;

    v.type = ValueType::Nil;

    return v;
  }

  static Value undefined_value()
  {
    Value v;

    v.type = ValueType::Undefined;

    return v;
  }

  static Value number_value(const ssize_t n)
  {
    Value v;

    v.type = ValueType::Number;
    v.number = n;

    return v;
  }

  static Value string_value(const std::string &s)
  {
    Value v;

    v.type = ValueType::String;
    v.string = s;

    return v;
  }

  static Value boolean_value(const bool b)
  {
    Value v;

    v.type = ValueType::Boolean;
    v.boolean = b;

    return v;
  }

  static Value object_value(const std::shared_ptr<std::unordered_map<std::string, Value> > &properties)
  {
    Value v;

    v.type = ValueType::Object;
    v.object.properties = properties;

    return v;
  }

  static Value builtin_function_value(NativeFn body)
  {
    Value v;

    v.type = ValueType::Function;
    v.function.builtin = std::move(body);
    v.function.kind = FunctionValue::Kind::Builtin;

    return v;
  }

  static Value user_function_value(FunctionExpr *declaration)
  {
    Value v;

    v.type = ValueType::Function;
    v.function.declaration = declaration;
    v.function.kind = FunctionValue::Kind::UserDefined;

    return v;
  }

  static Value range_value(const int start, const int end, const int step, const bool inclusive)
  {
    Value v;

    v.type = ValueType::Range;
    v.range.start = start;
    v.range.end = end;
    v.range.step = step;
    v.range.inclusive = inclusive;

    return v;
  }

  static Value array_value(const ValueType type, const std::shared_ptr<std::vector<Value> > &elements)
  {
    Value v;

    v.type = ValueType::Array;
    v.array.type = type;
    v.array.elements = elements;

    return v;
  }

  static Value struct_value(const std::string &name, const std::unordered_map<std::string, ValueType> &fields)
  {
    Value v;

    v.type = ValueType::Struct;
    v.struct_definition.name = name;
    v.struct_definition.fields = fields;

    return v;
  }

  static Value struct_instance_value(const std::shared_ptr<StructDefinition> &def,
                                     const std::shared_ptr<std::unordered_map<std::string, Value> > &fields)
  {
    Value v;

    v.type = ValueType::Struct;
    v.struct_instance.definition = def;
    v.struct_instance.fields = fields;

    return v;
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
      case ValueType::Struct:
        return "Struct";
      default:
        return "Unknown";
    }
  }

  static ValueType type_of(const std::optional<std::string> &value, const std::set<std::string> *seen_types)
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
    if (*value == "Struct")
    {
      return ValueType::Struct;
    }

    if (seen_types && seen_types->contains(value.value()))
    {
      return ValueType::Struct;
    }

    ErrorService::runtime_error("Unknown type", *value);
    return ValueType::Nil;
  }
};

struct ReturnSignal
{
  Value value;
};
