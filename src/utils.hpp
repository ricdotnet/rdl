#pragma once

#include <string>
#include "./interpreter.hpp"

class Utils
{
public:
  static std::string normalise_identifier(std::string identifier);

  static std::vector<Value> split(const std::string &string, const std::string &delim);
};

template<typename T>
std::shared_ptr<T> native(const Value &v)
{
  return std::static_pointer_cast<T>(v.object.native_object);
}
