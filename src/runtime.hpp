#pragma once

#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "./interpreter.hpp"

class Runtime
{
public:
  Runtime() : out(&std::cout) {}

  std::ostream *out;

  std::unordered_map<std::string, Value> globals;

  void init_type_methods();

  void add_global(const std::string &name, const Value &value);

  Value resolve(const std::string &name);
};
