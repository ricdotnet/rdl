#include "./time_module.hpp"
#include "./time_lambdas.hpp"

[[nodiscard]] std::string TimeModule::name() const
{
  return "time";
}

[[nodiscard]] Value TimeModule::init()
{
  const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

  functions->emplace("now", Value::builtin_function_value(now()));
  functions->emplace("sleep", Value::builtin_function_value(sleep()));

  return Value::object_value(functions);
}
