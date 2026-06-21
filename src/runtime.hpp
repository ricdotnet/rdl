#pragma once

#include <iostream>
#include <unordered_map>
#include "./ast.hpp"
#include "./interpreter.hpp"
#include "./http/router.hpp"

class Runtime
{
public:
  Runtime() : out(&std::cout) {}

  std::ostream *out;

  std::unordered_map<std::string, Value> globals;

  Router router;

  void init_type_methods();

  void add_global(const std::string &name, const Value &value);

  Value resolve(const std::string &name);
};
