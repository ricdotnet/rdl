#pragma once

#include <string>

enum class TokenType
{
  Func,
  Let,
  If,
  Else,
  ElseIf,
  Identifier,
  Number,
  String,
  Plus,
  Minus,
  Star,
  Slash,
  Equal,
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  Comma,
  Semicolon,
  DoubleQuote,
  Concat,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
  EqualEqual,
  Bang,
  BangEqual,
  Return,
  EndOfFile,
  Unknown,
};

struct Token
{
  TokenType type;

  std::string value;

  int line;

  int column;
};

TokenType get_token_type(char c);

std::string token_type_to_string(TokenType type);
