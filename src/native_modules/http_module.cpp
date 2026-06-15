#include "./http_module.hpp"
#include "./http_lambdas.hpp"


[[nodiscard]] std::string HttpModule::name() const
{
  return "http";
}

[[nodiscard]] Value HttpModule::init()
{
  const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

  functions->emplace("listen", Value::builtin_function_value(listen(this->context)));

  return Value::object_value(functions);
}
