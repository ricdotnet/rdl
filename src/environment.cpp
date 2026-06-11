#include "./environment.hpp"
#include "./error_service.hpp"
#include "./interpreter.hpp"

void Environment::define(const std::string &name, const Value &value)
{
  std::string name_copy = name;
  bool is_mutable = false;

  if (!name.empty() && name[0] == '$')
  {
    is_mutable = true;
    name_copy = name.substr(1);
  }

  if (values.contains(name_copy))
  {
    ErrorService::runtime_error("Variable already defined", name_copy);
  }

  values[name_copy] = {value, is_mutable};
}

void Environment::remove(const std::string &name)
{
  std::string name_copy = name;

  if (!name.empty() && name[0] == '$')
  {
    name_copy = name.substr(1);
  }

  if (!values.contains(name_copy))
  {
    ErrorService::runtime_error("Variable not defined", name_copy);
  }

  values.erase(name_copy);
}

Value Environment::get(const std::string &name)
{
  if (values.contains(name))
  {
    return values.at(name).value;
  }

  if (parent)
  {
    return parent->get(name);
  }

  const auto *run = get_runtime();
  if (!run)
  {
    ErrorService::runtime_error("No runtime found", "");
    return Value::undefined_value();
  }

  if (run->globals.contains(name))
  {
    return run->globals.at(name);
  }

  return Value::undefined_value();
}

void Environment::assign(const std::string &name, const Value &value)
{
  if (const auto current = values.find(name); current != values.end())
  {
    if (!current->second.is_mutable)
    {
      ErrorService::runtime_error("Cannot reassign to immutable variable", name);
    }

    current->second.value = value;
    return;
  }

  if (parent)
  {
    parent->assign(name, value);
    return;
  }

  ErrorService::runtime_error("Undefined variable", name);
}

void Environment::set_runtime(Runtime &run)
{
  this->runtime = &run;
}

Runtime *Environment::get_runtime() const
{
  if (runtime)
  {
    return runtime;
  }
  if (parent)
  {
    return parent->get_runtime();
  }
  ErrorService::runtime_error("No runtime found", "");
  return nullptr;
}
