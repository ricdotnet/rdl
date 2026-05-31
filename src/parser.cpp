#include "./parser.hpp"

#include <memory>
#include <utility>
#include "./ast.hpp"
#include "./error_service.hpp"
#include "./token.hpp"

Parser::Parser(std::vector<Token> t) : tokens(std::move(t)) {}

std::vector<std::unique_ptr<Expr> > Parser::parse()
{
  std::vector<std::unique_ptr<Expr> > statements;

  while (!is_at_end())
  {
    statements.push_back(declaration());

    if (try_consume(TokenType::Semicolon)) continue;
  }

  return statements;
}

std::unique_ptr<Expr> Parser::declaration()
{
  if (match(TokenType::Func))
  {
    auto name = consume(TokenType::Identifier);
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
        auto token = consume(TokenType::Identifier);
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
    auto token = consume(TokenType::Identifier);
    consume(TokenType::Equal);

    if (match(TokenType::LeftBrace))
    {
      auto value = object();
      return std::make_unique<LetExpr>(token.value, std::move(value));
    }

    auto initialiser = expression();
    return std::make_unique<LetExpr>(token.value, std::move(initialiser));
  }

  if (check(TokenType::Identifier) && peek_next().type == TokenType::Equal)
  {
    auto token = consume(TokenType::Identifier);
    consume(TokenType::Equal);

    auto value = expression();
    return std::make_unique<AssignExpr>(token.value, std::move(value));
  }

  if (match(TokenType::If))
  {
    return if_statement();
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
    return for_loop();
  }

  return expression();
}

std::unique_ptr<Expr> Parser::if_statement()
{
  consume(TokenType::LeftParen);

  auto condition = expression();

  consume(TokenType::RightParen);
  consume(TokenType::LeftBrace);

  auto then_branch = block();
  std::unique_ptr<Expr> else_branch = nullptr;

  if (match(TokenType::ElseIf))
  {
    else_branch = if_statement();
  } else if (match(TokenType::Else))
  {
    consume(TokenType::LeftBrace);
    else_branch = block();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Expr> Parser::for_loop()
{
  auto identifier_token = consume(TokenType::Identifier);
  consume(TokenType::In);

  Expr *expr = nullptr;

  // TODO: for handling ArrayExpr in the future
  if (peek().type != TokenType::Number)
  {
    return nullptr;
  } else
  {
    const auto init = consume(TokenType::Number).value;
    consume(TokenType::Range);
    const auto end = consume(TokenType::Number).value;
    std::string step = "1";

    if (peek().type == TokenType::Comma)
    {
      consume(TokenType::Comma);
      step = consume(TokenType::Number).value;
    }

    expr = new RangeExpr(std::stoi(init), std::stoi(end), std::stoi(step));
  }

  consume(TokenType::LeftBrace);
  auto body = block();

  return std::make_unique<ForStmt>(std::move(identifier_token.value), std::unique_ptr<Expr>(expr), std::move(body));
}

std::unique_ptr<BlockStmt> Parser::block()
{
  std::vector<std::unique_ptr<Expr> > statements;

  while (!check(TokenType::RightBrace) && !is_at_end())
  {
    statements.push_back(statement());

    try_consume(TokenType::Semicolon);
  }

  consume(TokenType::RightBrace);

  return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::object()
{
  std::unordered_map<std::string, std::unique_ptr<Expr> > properties;

  while (!check(TokenType::RightBrace) && !is_at_end())
  {
    auto key = consume(TokenType::Identifier);
    consume(TokenType::Colon);

    if (check(TokenType::LeftBrace))
    {
      consume(TokenType::LeftBrace);
      auto value = object();
      properties.emplace(key.value, std::move(value));
      try_consume(TokenType::Comma);
      continue;
    }

    auto value = expression();
    properties.emplace(key.value, std::move(value));
    try_consume(TokenType::Comma);
  }

  consume(TokenType::RightBrace);

  return std::make_unique<ObjectExpr>(std::move(properties));
}

std::unique_ptr<Expr> Parser::expression()
{
  if (match(TokenType::Semicolon))
  {
    return nullptr;
  }

  auto expr = equality();

  if (peek().type == TokenType::And)
  {
    const auto oper = consume(TokenType::And);
    auto right = equality();

    expr = std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
  } else if (peek().type == TokenType::Or)
  {
    const auto oper = consume(TokenType::Or);
    auto right = equality();

    expr = std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::equality()
{
  auto expr = concat();

  while (match(TokenType::EqualEqual) || match(TokenType::BangEqual))
  {
    const auto oper = previous();
    auto right = concat();

    expr = std::make_unique<BinaryExpr>(std::move(expr), oper, std::move(right));
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
  auto expr = unary();

  while (match(TokenType::Star) || match(TokenType::Slash))
  {
    Token op = previous();
    auto right = unary();

    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
  if (match(TokenType::Bang) || match(TokenType::Minus))
  {
    const auto token = previous();
    auto value = expression();
    return std::make_unique<UnaryExpr>(token, std::move(value));
  }

  return postfix();
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
        return nullptr;
      }

      expr = std::make_unique<CallExpr>(var->name, std::move(args));

      continue;
    }

    if (match(TokenType::Dot))
    {
      const auto method = consume(TokenType::Identifier);

      if (peek_next().type != TokenType::LeftParen)
      {
        // We can assume object access here
        expr = std::make_unique<DotExpr>(method.value, std::move(expr));
        continue;
      }

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
    const auto token = previous();

    return std::make_unique<NumberExpr>(std::stoi(token.value));
  }

  if (match(TokenType::Identifier))
  {
    const auto token = previous();

    return std::make_unique<VariableExpr>(token.value);
  }

  if (match(TokenType::String))
  {
    const auto token = previous();

    return std::make_unique<StringExpr>(token.value);
  }

  if (match(TokenType::True) || match(TokenType::False))
  {
    const auto token = previous();

    return std::make_unique<BooleanExpr>(token.type == TokenType::True);
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

bool Parser::match(const TokenType type)
{
  if (check(type))
  {
    advance();
    return true;
  }

  return false;
}

Token Parser::peek() { return tokens[current]; }

Token Parser::peek_next()
{
  if (current + 1 >= tokens.size())
  {
    return tokens.back();
  }
  return tokens[current + 1];
}

bool Parser::is_at_end() { return peek().type == TokenType::EndOfFile; }

Token Parser::advance()
{
  if (!is_at_end())
  {
    current++;
  }

  return previous();
}

Token Parser::previous(const int prev) { return tokens[current - prev]; }

bool Parser::check(const TokenType type)
{
  if (is_at_end())
  {
    return false;
  }

  return peek().type == type;
}

Token Parser::consume(const TokenType type)
{
  if (check(type))
  {
    auto token = advance();
    return token;
  }

  auto token = peek();
  ErrorService::syntax_error("Unexpected token", token);

  // we exit the program before reaching here
  return token;
}

bool Parser::try_consume(const TokenType type)
{
  if (check(type))
  {
    advance();
    return true;
  }

  return false;
}
