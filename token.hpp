#pragma once

#include <string>

enum class TokenType {
    Identifier,
    Number,
    String,
    Plus,
    Minus,
    Star,
    Slash,
    Equals,
    LeftParen,
    RightParen,
    Comma,
    Semicolon,
    DoubleQuote,
    EndOfFile,
};

struct Token {
    TokenType type;
    std::string value;
};

TokenType get_token_type(char c);

std::string token_type_to_string(TokenType type);