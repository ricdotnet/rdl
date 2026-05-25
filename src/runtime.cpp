#include "./runtime.hpp"
#include <sstream>
#include "./error_service.hpp"

void Runtime::define_builtin(const std::string &name, const builtin_function &function) {
  if (builtins.contains(name)) {
    ErrorService::runtime_error("Builtin function already defined", name);
  }

  builtins[name] = function;
}
