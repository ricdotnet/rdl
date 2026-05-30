#include "./environment.hpp"

#include <iostream>

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
  Value value;

  if (!values.contains(name))
  {
    // Check parent if identifier does not exist in the current scope
    if (parent)
    {
      if (parent->values.contains(name))
      {
        return parent->get(name);
      }
    }

    return Value::undefined_value();
  }

  return values.at(name).value;
}

void Environment::assign(const std::string &name, const Value &value)
{
  const auto current = values.find(name);

  if (current == values.end())
  {
    // Assign in parent scope if exists
    if (parent)
    {
      if (parent->values.contains(name))
      {
        parent->assign(name, value);
        return;
      }
    }

    ErrorService::runtime_error("Undefined variable", name);
  }

  if (!current->second.is_mutable)
  {
    ErrorService::runtime_error("Cannot reassign to immutable variable", name);
  }

  current->second.value = value;
}
