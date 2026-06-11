#include "./io_module.hpp"
#include "./io_lambdas.hpp"

[[nodiscard]] std::string IoModule::name() const
{
  return "io";
}

[[nodiscard]] Value IoModule::init()
{
  const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

  functions->emplace("print", Value::builtin_function_value(print(context)));
  functions->emplace("println", Value::builtin_function_value(println(context)));
  functions->emplace("input", Value::builtin_function_value(input()));

  return Value::object_value(functions);
}
