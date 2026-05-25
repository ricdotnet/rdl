#pragma once

#include <string>
#include "./token.hpp"

class ErrorService
{
public:
  static void runtime_error(const std::string &message, const std::string &context);

  static void syntax_error(const std::string &message, const Token &token);
};
