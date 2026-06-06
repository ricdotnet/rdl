#pragma once

#include <string>

enum class TokenType
{
  Func,
  Let,
  Struct,
  If,
  Else,
  ElseIf,
  For,
  In,
  Range,
  While,
  Identifier,
  Number,
  String,
  True,
  False,
  And,
  Or,
  Plus,
  Minus,
  Star,
  Slash,
  Equal,
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
  Comma,
  Semicolon,
  SingleQuote,
  DoubleQuote,
  Backtick,
  Concat,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
  EqualEqual,
  Bang,
  BangEqual,
  Return,
  Comment,
  Dot,
  Colon,
  ColonColon,
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
