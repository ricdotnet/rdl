#include "error_service.hpp"

#include <iostream>

void ErrorService::syntax_error(
    const std::string &message,
    const Token &token
) {
    std::cerr << "Error: " << message << "\n";
    std::cerr << "  at line " << token.line
            << ", column " << token.column << "\n\n";

    std::exit(1);
}
