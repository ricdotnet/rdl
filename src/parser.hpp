#pragma once

#include <memory>
#include <vector>
#include "./ast.hpp"
#include "./token.hpp"

class Parser
{
private:
  std::vector<Token> tokens;

  size_t current = 0;

public:
  explicit Parser(std::vector<Token> t);

  std::vector<std::unique_ptr<Expr> > parse();

private:
  std::unique_ptr<Expr> statement();

  std::unique_ptr<Expr> ifStatement();

  std::unique_ptr<Expr> block();

  std::unique_ptr<Expr> expression();

  std::unique_ptr<Expr> equality();

  std::unique_ptr<Expr> comparison();

  std::unique_ptr<Expr> concat();

  std::unique_ptr<Expr> term();

  std::unique_ptr<Expr> factor();

  std::unique_ptr<Expr> postfix();

  std::unique_ptr<Expr> primary();

  Token peek();

  Token peekNext();

  Token advance();

  Token previous();

  bool match(TokenType type);

  bool check(TokenType type);

  bool isAtEnd();

  void consume(TokenType type);
};
