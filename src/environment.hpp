#pragma once

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
public:
  std::unordered_map<std::string, Binding> values;

  void define(const std::string &name, const Value &value);

  Value get(const std::string &name);

  void assign(const std::string &name, const Value &value);
};
