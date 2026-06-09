#pragma once

#include <string>
#include "./interpreter.hpp"

class Utils
{
public:
  static std::string normalise_identifier(std::string identifier);

  static std::vector<Value> split(const std::string &string, const std::string &delim);
};
