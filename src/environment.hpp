#pragma once

#include <string>
#include <unordered_map>
#include "./interpreter.hpp"
#include "./runtime.hpp"

struct Binding
{
  Value value;

  bool is_mutable{};
};

class Environment
{
private:
  Runtime *runtime{};

public:
  Environment *parent{};

  Environment() = default;

  explicit Environment(Environment *parent) : parent(parent) {}

  std::unordered_map<std::string, Binding> values;

  void define(const std::string &name, const Value &value);

  void remove(const std::string &name);

  Value get(const std::string &name);

  void assign(const std::string &name, const Value &value);

  void set_runtime(Runtime &run);

  Runtime *get_runtime() const;
};
