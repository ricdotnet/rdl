#include "error_service.hpp"
#include <iostream>

void ErrorService::runtime_error(const std::string &message, const std::string &context) {
  std::cerr << "Error: runtime error - " << message << "\n";
  std::cerr << "Context: " << context << "\n";

  std::exit(1);
}

void ErrorService::syntax_error(const std::string &message, const Token &token) {
  std::cerr << "Error: " << message << "\n";
  std::cerr << "  token: " << token.value << "\n";
  std::cerr << "  at line " << token.line << ", column " << token.column << "\n\n";

  std::exit(1);
}
