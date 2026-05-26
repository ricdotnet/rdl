#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include "./interpreter.hpp"

class Runtime
{
private:
  void init_type_methods();

public:
  Runtime();

  using builtin_function = std::function<Value(const std::vector<Value> &)>;

  using type_method = std::function<Value(const Value &, const std::vector<Value> &)>;

  std::unordered_map<std::string, builtin_function> builtins;

  std::unordered_map<Value::Type, std::unordered_map<std::string, type_method> > type_methods;

  void define_builtin(const std::string &name, const builtin_function &function);

  void define_type_method(Value::Type type, const std::string &method_name, const type_method &method);
};
