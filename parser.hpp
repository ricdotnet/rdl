#pragma once

#include "ast.hpp"
#include "token.hpp"

#include <memory>
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

public:
    Parser(std::vector<Token> t);

    std::vector<std::unique_ptr<Expr> > parse();

    std::unique_ptr<Expr> statement();

    std::unique_ptr<Expr> expression();

    std::unique_ptr<Expr> term();

private:
    std::unique_ptr<Expr> factor();

    std::unique_ptr<Expr> primary();

    std::unique_ptr<Expr> postfix();

    Token peek();

    Token peekNext();

    Token advance();

    Token previous();

    bool match(TokenType type);

    bool check(TokenType type);

    bool isAtEnd();

    void consume(TokenType type);
};
