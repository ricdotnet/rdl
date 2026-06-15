#pragma once

#include <string>

enum class TokenType
{
  Unknown,
  Comment,
  EndOfFile,

  // IDENTIFIER LITERALS
  Identifier,
  Number,
  String,
  True,
  False,
  HttpVerb,

  // OPERATORS
  And,
  Or,
  Plus,
  Minus,
  Star,
  Slash,
  Equal,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
  EqualEqual,
  Bang,
  BangEqual,

  // DELIMITERS
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
  Dot,
  Colon,
  ColonColon,

  // KEYWORDS
  Import,
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
  Return,
  Group,
  Route,
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
