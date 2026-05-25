#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include "./interpreter.hpp"

struct Binding
{
  Value value;

  bool is_mutable{};
};

class Environment
{
private:
  using builtin_function = std::function<Value(const std::vector<Value> &)>;

public:
  std::unordered_map<std::string, Binding> values;

  std::unordered_map<std::string, builtin_function> builtins;

  void define(const std::string &name, const Value &value);

  Value get(const std::string &name);

  void assign(const std::string &name, const Value &value);

  void define_builtin(const std::string &name, const builtin_function &function);
};
