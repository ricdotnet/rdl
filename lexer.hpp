#pragma once

#include "token.hpp"

#include <vector>

class Lexer {
    std::string source;
    size_t current = 0;

    static bool is_digit(char c);

    static bool is_alpha(char c);

public:
    Lexer(std::string src);

    std::vector<Token> tokenize();
};
