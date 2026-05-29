#include "./parser.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include "./ast.hpp"
#include "./error_service.hpp"
#include "./token.hpp"

Parser::Parser(std::vector<Token> t) : tokens(std::move(t)) {}

std::vector<std::unique_ptr<Expr> > Parser::parse()
{
  std::vector<std::unique_ptr<Expr> > statements;

  while (!isAtEnd())
  {
    statements.push_back(declaration());

    if (match(TokenType::Semicolon))
    {
      continue;
    }
  }

  return statements;
}

std::unique_ptr<Expr> Parser::declaration()
{
  if (match(TokenType::Func))
  {
    Token name = consume(TokenType::Identifier);
    auto receiver_type = std::optional<std::string>();

    if (match(TokenType::ColonColon))
    {
      receiver_type = name.value;
      name = consume(TokenType::Identifier);
    }

    consume(TokenType::LeftParen);

    std::vector<std::string> params;

    if (!check(TokenType::RightParen))
    {
      do
      {
        Token token = consume(TokenType::Identifier);
        params.push_back(token.value);
      } while (match(TokenType::Comma));
    }

    consume(TokenType::RightParen);
    consume(TokenType::LeftBrace);

    auto body = block();

    return std::make_unique<FunctionExpr>(name.value, std::move(params), std::move(body), std::move(receiver_type));
  }

  return statement();
}

std::unique_ptr<Expr> Parser::statement()
{
  if (match(TokenType::Let))
  {
    Token token = advance();
    consume(TokenType::Equal);

    auto initialiser = expression();
    return std::make_unique<LetExpr>(token.value, std::move(initialiser));
  }

  if (check(TokenType::Identifier) && peekNext().type == TokenType::Equal)
  {
    Token token = advance();
    advance();

    auto value = expression();
    return std::make_unique<AssignExpr>(token.value, std::move(value));
  }

  if (match(TokenType::If))
  {
    return ifStatement();
  }

  if (match(TokenType::While))
  {
    consume(TokenType::LeftParen);
    auto condition = expression();
    consume(TokenType::RightParen);
    consume(TokenType::LeftBrace);
    auto body = block();
    return std::make_unique<WhileExpr>(std::move(condition), std::move(body));
  }

  if (match(TokenType::Return))
  {
    auto value = expression();
    return std::make_unique<ReturnStmt>(std::move(value));
  }

  if (match(TokenType::For))
  {
    std::cout << "For loop" << std::endl;
    return forLoop();
  }

  return expression();
}

std::unique_ptr<Expr> Parser::ifStatement()
{
  consume(TokenType::LeftParen);

  auto condition = expression();

  consume(TokenType::RightParen);
  consume(TokenType::LeftBrace);

  auto then_branch = block();
  std::unique_ptr<Expr> else_branch = nullptr;

  if (match(TokenType::ElseIf))
  {
    else_branch = ifStatement();
  } else if (match(TokenType::Else))
  {
    consume(TokenType::LeftBrace);
    else_branch = block();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Expr> Parser::forLoop()
{
  auto identifier_token = consume(TokenType::Identifier);
  consume(TokenType::In);
  const auto init = consume(TokenType::Number).value;
  consume(TokenType::Concat);
  const auto end = consume(TokenType::Number).value;

  std::string step = "1";

  if (peek().type == TokenType::Comma)
  {
    consume(TokenType::Comma);
    step = consume(TokenType::Number).value;
  }

  consume(TokenType::LeftBrace);
  auto body = block();

  return std::make_unique<ForStmt>(std::move(identifier_token.value), std::stoi(init), std::stoi(end), std::stoi(step),
                                   std::move(body));
}

std::unique_ptr<BlockStmt> Parser::block()
{
  std::vector<std::unique_ptr<Expr> > statements;

  while (!check(TokenType::RightBrace) && !isAtEnd())
  {
    statements.push_back(statement());

    if (match(TokenType::Semicolon))
    {
      continue;
    }
  }

  consume(TokenType::RightBrace);

  return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression()
{
  if (match(TokenType::Semicolon))
  {
    return nullptr;
  }

  return equality();
}

std::unique_ptr<Expr> Parser::equality()
{
  auto expr = concat();

  while (match(TokenType::EqualEqual) || match(TokenType::BangEqual))
  {
    Token op = previous();
    auto right = concat();

    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::concat()
{
  auto expr = comparison();

  while (match(TokenType::Concat))
  {
    Token op = previous();
    auto right = comparison();

    expr = std::make_unique<ConcatExpr>(std::move(expr), std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
  auto expr = term();

  while (match(TokenType::Less) || match(TokenType::LessEqual) || match(TokenType::Greater) || match(
           TokenType::GreaterEqual))
  {
    Token op = previous();
    auto right = term();

    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::term()
{
  auto expr = factor();

  while (match(TokenType::Plus) || match(TokenType::Minus))
  {
    Token op = previous();
    auto right = factor();

    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
  auto expr = postfix();

  while (match(TokenType::Star) || match(TokenType::Slash))
  {
    Token op = previous();
    auto right = postfix();

    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::postfix()
{
  auto expr = primary();

  // TODO: understand this
  while (true)
  {
    if (match(TokenType::LeftParen))
    {
      std::vector<std::unique_ptr<Expr> > args;

      if (!check(TokenType::RightParen))
      {
        do
        {
          args.push_back(expression());
        } while (match(TokenType::Comma));
      }

      consume(TokenType::RightParen);

      // only valid if base is a variable for now
      auto *var = dynamic_cast<VariableExpr *>(expr.get());

      if (!var)
      {
        ErrorService::runtime_error("Only identifiers can be called", "");
      }

      expr = std::make_unique<CallExpr>(var->name, std::move(args));

      continue;
    }

    if (match(TokenType::Dot))
    {
      Token method = consume(TokenType::Identifier);

      // We don't need any argument support for now, so just consume left and right parents
      consume(TokenType::LeftParen);
      consume(TokenType::RightParen);

      expr = std::make_unique<MethodCallExpr>(std::move(expr), method.value, std::vector<std::unique_ptr<Expr> >());

      continue;
    }

    break;
  }

  return expr;
}

std::unique_ptr<Expr> Parser::primary()
{
  if (match(TokenType::Number))
  {
    Token token = previous();

    return std::make_unique<NumberExpr>(std::stoi(token.value));
  }

  if (match(TokenType::Identifier))
  {
    Token token = previous();

    return std::make_unique<VariableExpr>(token.value);
  }

  if (match(TokenType::String))
  {
    Token token = previous();

    return std::make_unique<StringExpr>(token.value);
  }

  if (match(TokenType::LeftParen))
  {
    auto expr = expression();
    consume(TokenType::RightParen);
    return expr;
  }

  ErrorService::syntax_error("Expected expression", tokens[current]);
  return nullptr;
}

bool Parser::match(TokenType type)
{
  if (check(type))
  {
    advance();
    return true;
  }

  return false;
}

Token Parser::peek() { return tokens[current]; }

Token Parser::peekNext()
{
  if (current + 1 >= tokens.size())
  {
    return tokens.back();
  }
  return tokens[current + 1];
}

bool Parser::isAtEnd() { return peek().type == TokenType::EndOfFile; }

Token Parser::advance()
{
  if (!isAtEnd())
  {
    current++;
  }

  return previous();
}

Token Parser::previous() { return tokens[current - 1]; }

bool Parser::check(TokenType type)
{
  if (isAtEnd())
  {
    return false;
  }

  return peek().type == type;
}

Token Parser::consume(TokenType type)
{
  if (check(type))
  {
    auto token = advance();
    return token;
  }

  ErrorService::syntax_error("Unexpected token", peek());
}
