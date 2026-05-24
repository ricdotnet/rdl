#include "token.hpp"

std::string token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Number: return "Number";
        case TokenType::String: return "String";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Equals: return "Equals";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::Comma: return "Comma";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::DoubleQuote: return "DoubleQuote";
        case TokenType::EndOfFile: return "EndOfFile";
        default: return "Invalid TokenType";
    }
}
