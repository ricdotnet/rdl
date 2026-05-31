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
  std::unique_ptr<Expr> declaration();

  std::unique_ptr<Expr> statement();

  std::unique_ptr<Expr> if_statement();

  std::unique_ptr<Expr> for_loop();

  std::unique_ptr<BlockStmt> block();

  std::unique_ptr<Expr> object();

  std::unique_ptr<Expr> expression();

  std::unique_ptr<Expr> assignment();

  std::unique_ptr<Expr> equality();

  std::unique_ptr<Expr> comparison();

  std::unique_ptr<Expr> concat();

  std::unique_ptr<Expr> term();

  std::unique_ptr<Expr> factor();

  std::unique_ptr<Expr> unary();

  std::unique_ptr<Expr> postfix();

  std::unique_ptr<Expr> primary();

  Token peek();

  Token peek_next();

  Token advance();

  Token previous(int prev = 1);

  bool match(TokenType type);

  bool check(TokenType type);

  bool is_at_end();

  Token consume(TokenType type);

  bool try_consume(TokenType type);
};
