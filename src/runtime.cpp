#include "./runtime.hpp"
#include "./environment.hpp"
#include "./error_service.hpp"
#include "./utils/string.hpp"

void Runtime::add_global(const std::string &name, const Value &value)
{
  if (globals.contains(name))
  {
    ErrorService::runtime_error("Global variable " + name + " already defined", name);
  }

  globals[name] = value;
}

void Runtime::init_type_methods()
{
  const auto string_methods = Value::object_value(std::make_shared<std::unordered_map<std::string, Value> >());
  const auto number_methods = Value::object_value(std::make_shared<std::unordered_map<std::string, Value> >());
  const auto boolean_methods = Value::object_value(std::make_shared<std::unordered_map<std::string, Value> >());
  const auto array_methods = Value::object_value(std::make_shared<std::unordered_map<std::string, Value> >());

  string_methods.object.properties->insert({
    "length", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (!args.empty())
      {
        ErrorService::runtime_error("Expected 0 arguments for string length method.",
                                    "Found " + std::to_string(args.size()));
      }

      return Value::number_value(static_cast<int>(receiver.string.length()));
    })
  });

  string_methods.object.properties->insert({
    "upper", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (!args.empty())
      {
        ErrorService::runtime_error("Expected 0 arguments for string upper method.",
                                    "Found " + std::to_string(args.size()));
      }

      return Value::string_value(to_upper(receiver.string));
    })
  });

  string_methods.object.properties->insert({
    "lower", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (!args.empty())
      {
        ErrorService::runtime_error("Expected 0 arguments for string lower method.",
                                    "Found " + std::to_string(args.size()));
      }

      return Value::string_value(to_lower(receiver.string));
    })
  });

  string_methods.object.properties->insert({
    "split", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (args.size() > 1)
      {
        ErrorService::runtime_error("Expected no more than 1 argument for string split method.",
                                    "Found " + std::to_string(args.size()));
      }

      std::string delimiter;
      if (!args.empty())
      {
        delimiter = args[0].string;
      }

      const auto split_strings = split(receiver.string, delimiter);
      return Value::array_value(ValueType::String,
                                std::make_shared<std::vector<Value> >(split_strings.begin(), split_strings.end()));
    })
  });

  number_methods.object.properties->insert({
    "to_string", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (!args.empty())
      {
        ErrorService::runtime_error("Expected 0 arguments for number to_string method.",
                                    "Found " + std::to_string(args.size()));
      }
      return Value::string_value("\"" + std::to_string(receiver.number) + "\"");
    })
  });

  boolean_methods.object.properties->insert({
    "to_string", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (!args.empty())
      {
        ErrorService::runtime_error("Expected 0 arguments for boolean to_string method.",
                                    "Found " + std::to_string(args.size()));
      }
      return Value::string_value(receiver.boolean ? "\"true\"" : "\"false\"");
    })
  });

  array_methods.object.properties->insert({
    "push", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (args.empty() || args.size() > 1)
      {
        ErrorService::runtime_error("Expected 1 argument for array push method.",
                                    "Found " + std::to_string(args.size()));
      }

      if (receiver.array.type != args[0].type)
      {
        ErrorService::runtime_error("Type mismatch in array push method.",
                                    "Expected " + Value::type_name(receiver.array.type) + ", got " + Value::type_name(
                                      args[0].type));
      }

      receiver.array.elements->push_back(args[0]);

      return receiver;
    })
  });

  array_methods.object.properties->insert({
    "pop", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (!args.empty())
      {
        ErrorService::runtime_error("Expected 0 arguments for array pop method.",
                                    "Found " + std::to_string(args.size()));
      }

      if (receiver.array.elements->empty())
      {
        ErrorService::runtime_error("Cannot pop from empty array", "");
      }

      const auto last_element = receiver.array.elements->back();
      receiver.array.elements->pop_back();

      return last_element;
    })
  });

  globals["String"] = string_methods;
  globals["Number"] = number_methods;
  globals["Boolean"] = boolean_methods;
  globals["Array"] = array_methods;
}
