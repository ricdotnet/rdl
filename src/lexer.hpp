#pragma once

#include <vector>
#include "./token.hpp"

class Lexer
{
  std::string source;

  size_t current = 0;

  int line = 1;

  int column = 1;

  static bool is_digit(char c);

  static bool is_alpha(char c);

  static char peek(const std::string &source, size_t index);

public:
  explicit Lexer(std::string src);

  std::vector<Token> tokenize();
};
