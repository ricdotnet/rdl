#include "./fs_module.hpp"
#include <sstream>
#include "./fs_lambdas.hpp"

[[nodiscard]] std::string FileSystemModule::name() const
{
  return "fs";
}

[[nodiscard]] Value FileSystemModule::init()
{
  const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

  functions->emplace("read_file", Value::builtin_function_value(read_file()));
  functions->emplace("write_file", Value::builtin_function_value(write_file()));
  functions->emplace("open_file", Value::builtin_function_value(open_file()));

  return Value::object_value(functions);
}
