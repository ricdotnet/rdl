#include "./environment.hpp"
#include "./error_service.hpp"
#include "./interpreter.hpp"

void Environment::define(const std::string &name, const Value &value) {
  if (values.contains(name)) {
    ErrorService::runtime_error("Variable already defined", name);
  }

  std::string name_copy = name;
  bool is_mutable = false;

  if (name[0] == '$') {
    is_mutable = true;
    name_copy = name.substr(1);
  }

  values[name_copy] = {value, is_mutable};
}

Value Environment::get(const std::string &name) {
  if (!values.contains(name)) {
    return Value::undefined_value();
  }

  return values.at(name).value;
}

void Environment::assign(const std::string &name, const Value &value) {
  const auto current = values.find(name);

  if (current == values.end()) {
    ErrorService::runtime_error("Undefined variable", name);
  }

  if (!current->second.is_mutable) {
    ErrorService::runtime_error("Cannot reassign to immutable variable", name);
  }

  current->second.value = value;
}

void Environment::define_builtin(const std::string &name, const builtin_function &function) {
  if (builtins.contains(name)) {
    ErrorService::runtime_error("Builtin function already defined", name);
  }

  builtins[name] = function;
}
