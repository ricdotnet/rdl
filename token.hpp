#pragma once

#include <string>

enum class TokenType {
    Let,
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
    Concat,
    EndOfFile,
    Unknown,
};

struct Token {
    TokenType type;
    std::string value;

    size_t line;
    size_t column;
};

TokenType get_token_type(char c);

std::string token_type_to_string(TokenType type);