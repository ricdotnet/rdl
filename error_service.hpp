#pragma once

#include "token.hpp"

#include <string>

class ErrorService {
public:
    static void runtime_error(const std::string &message, const std::string &context);

    static void syntax_error(const std::string &message, const Token &token);
};
