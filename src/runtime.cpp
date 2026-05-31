#include "./runtime.hpp"
#include <iostream>
#include <sstream>
#include <thread>

#include "./error_service.hpp"
#include "environment.hpp"

Runtime::Runtime()
{
  init_type_methods();
}

void Runtime::init_builtins(Environment &env)
{
  env.define("print", Value::builtin_function_value([](const std::vector<Value> &args) -> Value {
    for (const auto &arg: args)
    {
      std::cout << arg.to_string() << " ";
    }
    std::cout << std::endl;
    return Value::nil_value();
  }));

  env.define("sleep", Value::builtin_function_value([](const std::vector<Value> &args) -> Value {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for sleep in milliseconds.",
                                  "Found " + std::to_string(args.size()));
    }

    const auto duration_value = &args[0];
    if (!duration_value->is_number())
    {
      ErrorService::runtime_error("Expected number in milliseconds for sleep duration.",
                                  "Found " + Value::type_name(duration_value->type));
    }

    const auto duration = duration_value->number;
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));

    return Value::nil_value();
  }));

  env.define("now", Value::builtin_function_value([](const std::vector<Value> &args) -> Value {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for now, found ", std::to_string(args.size()));
    }

    return Value::number_value(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).
      count());
  }));
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

  // user_methods[expr.receiver_type.value()][name] = &expr;
}
