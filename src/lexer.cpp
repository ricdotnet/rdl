#include "./lexer.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include "./error_service.hpp"
#include "./token.hpp"

bool Lexer::is_digit(const char c) { return std::isdigit(c); }

bool Lexer::is_alpha(const char c) { return std::isalpha(c); }

char Lexer::peek(const std::string &source, const size_t index) { return source[index]; }

Token Lexer::previous_token(const std::vector<Token> &tokens, const size_t index_back)
{
  // TODO: handle empty tokens list
  return tokens[tokens.size() - index_back];
}

Lexer::Lexer(std::string src) : source(std::move(src)) {};

std::vector<Token> Lexer::tokenize()
{
  std::vector<Token> tokens;

  while (current < source.size())
  {
    const char c = peek(source, current);

    if (c == '\n')
    {
      line++;
      column = 0;
    }

    if (std::isspace(c))
    {
      current++;
      column++;
      continue;
    }

    // comments... I got a token, but I don't think it will ever be necessary?
    if (c == '/')
    {
      if (peek(source, current + 1) == '/')
      {
        while (current < source.size() && peek(source, current) != '\n')
        {
          current++;
          column++;
        }
        continue;
      }
    }

    // identifiers started by $ will be mutable
    if (is_alpha(c) || c == '_' || c == '$')
    {
      std::string value;

      while (current < source.size() && (is_alpha(peek(source, current)) || is_digit(peek(source, current)) ||
                                         peek(source, current) == '_' || peek(source, current) == '$'))
      {
        value += peek(source, current);
        current++;
        column++;
      }

      if (value == "if")
      {
        tokens.push_back({TokenType::If, value, line, column});
        continue;
      }

      if (value == "else")
      {
        tokens.push_back({TokenType::Else, value, line, column});
        continue;
      }

      if (value == "elseif")
      {
        tokens.push_back({TokenType::ElseIf, value, line, column});
        continue;
      }

      if (value == "for")
      {
        tokens.push_back({TokenType::For, value, line, column});
        continue;
      }

      if (value == "in")
      {
        tokens.push_back({TokenType::In, value, line, column});
        continue;
      }

      if (value == "while")
      {
        tokens.push_back({TokenType::While, value, line, column});
        continue;
      }

      if (value == "let")
      {
        tokens.push_back({TokenType::Let, value, line, column});
        continue;
      }

      if (value == "func")
      {
        tokens.push_back({TokenType::Func, value, line, column});
        continue;
      }

      if (value == "return")
      {
        tokens.push_back({TokenType::Return, value, line, column});
        continue;
      }

      if (value == "true")
      {
        tokens.push_back({TokenType::True, value, line, column});
        continue;
      }

      if (value == "false")
      {
        tokens.push_back({TokenType::False, value, line, column});
        continue;
      }

      tokens.push_back({TokenType::Identifier, value, line, column});

      continue;
    }

    if (is_digit(c))
    {
      std::string number;

      while (current < source.size() && is_digit(peek(source, current)))
      {
        number += peek(source, current);
        current++;
        column++;
      }

      tokens.push_back({TokenType::Number, number, line, column});

      continue;
    }

    if (c == '"')
    {
      std::string value;
      current++;
      column++;

      while (current < source.size() && source[current] != '"')
      {
        value += source[current];
        current++;
        column++;
      }

      if (current >= source.size())
      {
        ErrorService::syntax_error("Unterminated string", {TokenType::String, value, line, column});
      }

      tokens.push_back({TokenType::String, value, line, column});
      current++;
      column++;

      continue;
    }

    if (c == '.' && peek(source, current + 1) == '.')
    {
      if (previous_token(tokens, 2).type == TokenType::In)
      {
        tokens.push_back({TokenType::Range, "..", line, column});
      } else
      {
        tokens.push_back({TokenType::Concat, "..", line, column});
      }
      current += 2;
      column += 2;
      continue;
    }

    switch (c)
    {
      case '+':
        tokens.push_back({TokenType::Plus, "+", line, column});
        break;
      case '-':
        tokens.push_back({TokenType::Minus, "-", line, column});
        break;
      case '*':
        tokens.push_back({TokenType::Star, "*", line, column});
        break;
      case '/':
        tokens.push_back({TokenType::Slash, "/", line, column});
        break;
      case '(':
        tokens.push_back({TokenType::LeftParen, "(", line, column});
        break;
      case ')':
        tokens.push_back({TokenType::RightParen, ")", line, column});
        break;
      case '{':
        tokens.push_back({TokenType::LeftBrace, "{", line, column});
        break;
      case '}':
        tokens.push_back({TokenType::RightBrace, "}", line, column});
        break;
      case '[':
        tokens.push_back({TokenType::LeftBracket, "[", line, column});
        break;
      case ']':
        tokens.push_back({TokenType::RightBracket, "]", line, column});
        break;
      case '=':
        if (peek(source, current + 1) == '=')
        {
          tokens.push_back({TokenType::EqualEqual, "==", line, column});
          current++;
          column++;
          break;
        }
        tokens.push_back({TokenType::Equal, "=", line, column});
        break;
      case ',':
        tokens.push_back({TokenType::Comma, ",", line, column});
        break;
      case ';':
        tokens.push_back({TokenType::Semicolon, ";", line, column});
        break;
      case '>':
        if (peek(source, current + 1) == '=')
        {
          tokens.push_back({TokenType::GreaterEqual, ">=", line, column});
          current++;
          column++;
          break;
        }
        tokens.push_back({TokenType::Greater, ">", line, column});
        break;
      case '<':
        if (peek(source, current + 1) == '=')
        {
          tokens.push_back({TokenType::LessEqual, "<=", line, column});
          current++;
          column++;
          break;
        }
        tokens.push_back({TokenType::Less, "<", line, column});
        break;
      case '!':
        if (peek(source, current + 1) == '=')
        {
          tokens.push_back({TokenType::BangEqual, "!=", line, column});
          current++;
          column++;
          break;
        }
        tokens.push_back({TokenType::Bang, "!", line, column});
        break;
      case '.':
        tokens.push_back({TokenType::Dot, ".", line, column});
        break;
      case ':':
        if (peek(source, current + 1) == ':')
        {
          tokens.push_back({TokenType::ColonColon, "::", line, column});
          current++;
          column++;
          break;
        }
        tokens.push_back({TokenType::Colon, ":", line, column});
        break;
      case '&':
        if (peek(source, current + 1) == '&')
        {
          tokens.push_back({TokenType::And, "&&", line, column});
          current++;
          column++;
          break;
        }
        break;
      case '|':
        if (peek(source, current + 1) == '|')
        {
          tokens.push_back({TokenType::Or, "||", line, column});
          current++;
          column++;
          break;
        }
        break;
      default:
        ErrorService::syntax_error("Unexpected character: " + std::string(1, c),
                                   {TokenType::Unknown, std::string(1, c), line, column});
    }

    current++;
    column++;
  }

  tokens.push_back({TokenType::EndOfFile, "", line, column});

  return tokens;
}
