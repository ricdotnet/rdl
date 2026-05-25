#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include "./interpreter.hpp"

class Runtime
{
public:
  using builtin_function = std::function<Value(const std::vector<Value> &)>;

  std::unordered_map<std::string, builtin_function> builtins;

  void define_builtin(const std::string &name, const builtin_function &function);
};
