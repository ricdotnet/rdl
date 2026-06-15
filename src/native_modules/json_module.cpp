#include "./json_module.hpp"
#include "./json_lambdas.hpp"

[[nodiscard]] std::string JsonModule::name() const
{
  return "json";
}

[[nodiscard]] Value JsonModule::init()
{
  const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

  functions->emplace("marshal", Value::builtin_function_value(marshal_lambda(context)));
  functions->emplace("unmarshal", Value::builtin_function_value(unmarshal_lambda(context)));

  return Value::object_value(functions);
}
