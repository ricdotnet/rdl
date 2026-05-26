#include "./runtime.hpp"
#include <sstream>
#include "./error_service.hpp"

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
