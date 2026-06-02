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

  std::unordered_map<std::string, Value> globals;

  using type_method = std::function<Value(const Value &, const std::vector<Value> &)>;

  std::unordered_map<ValueType, std::unordered_map<std::string, type_method> > type_methods;

  std::unordered_map<ValueType, std::unordered_map<std::string, Value> > user_methods;

  static void init_builtins(Environment &);

  void define_type_method(ValueType type, const std::string &method_name, const type_method &method);

  void define_user_method(ValueType type, const std::string &method_name, const Value &value);
};
