#include "./runtime.hpp"
#include <sstream>
#include "./error_service.hpp"

Runtime::Runtime()
{
  init_type_methods();
}

void Runtime::init_type_methods()
{
  this->define_type_method(Value::String, "length", [](const Value &receiver, const std::vector<Value> &args) -> Value {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for string length method.",
                                  "Found " + std::to_string(args.size()));
    }

    return Value::number_value(static_cast<int>(receiver.string.length()));
  });

  this->define_type_method(Value::String, "upper", [](const Value &receiver, const std::vector<Value> &args) -> Value {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for string upper method.",
                                  "Found " + std::to_string(args.size()));
    }
    std::string upper = receiver.string;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return Value::string_value(upper);
  });

  this->define_type_method(Value::String, "lower", [](const Value &receiver, const std::vector<Value> &args) -> Value {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for string lower method.",
                                  "Found " + std::to_string(args.size()));
    }
    std::string lower = receiver.string;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return Value::string_value(lower);
  });

  this->define_type_method(Value::Number, "to_string",
                           [](const Value &receiver, const std::vector<Value> &args) -> Value {
                             if (!args.empty())
                             {
                               ErrorService::runtime_error("Expected 0 arguments for number to_string method.",
                                                           "Found " + std::to_string(args.size()));
                             }
                             return Value::string_value("\"" + std::to_string(receiver.number) + "\"");
                           });

  this->define_type_method(Value::Boolean, "to_string",
                           [](const Value &receiver, const std::vector<Value> &args) -> Value {
                             if (!args.empty())
                             {
                               ErrorService::runtime_error("Expected 0 arguments for boolean to_string method.",
                                                           "Found " + std::to_string(args.size()));
                             }
                             return Value::string_value(receiver.boolean ? "\"true\"" : "\"false\"");
                           });
}

void Runtime::define_builtin(const std::string &name, const builtin_function &function)
{
  if (builtins.contains(name))
  {
    ErrorService::runtime_error("Builtin function already defined", name);
  }

  builtins[name] = function;
}

void Runtime::define_type_method(Value::Type type, const std::string &method_name, const type_method &method)
{
  if (type_methods[type].contains(method_name))
  {
    ErrorService::runtime_error("Method already defined for this type", method_name);
  }

  type_methods[type][method_name] = method;
}

void Runtime::define_user_method(const std::string &name, FunctionExpr &expr)
{
  if (user_methods[expr.receiver_type.value()].contains(name))
  {
    ErrorService::runtime_error("User method already defined for this type", name);
  }

  user_methods[expr.receiver_type.value()][name] = &expr;
}
