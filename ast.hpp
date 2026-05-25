#pragma once

#include <memory>
#include <string>
#include <vector>
#include "token.hpp"
#include "visitor.hpp"

class Expr
{
public:
  virtual ~Expr() = default;

  virtual void accept(ExprVisitor &visitor) = 0;
};

class IfStmt : public Expr
{
public:
  std::unique_ptr<Expr> condition;

  std::unique_ptr<Expr> then_branch;

  std::unique_ptr<Expr> else_branch;

  explicit IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> then, std::unique_ptr<Expr> else_branch);

  void accept(ExprVisitor &visitor) override;
};

class BlockStmt : public Expr
{
public:
  std::vector<std::unique_ptr<Expr> > statements;

  explicit BlockStmt(std::vector<std::unique_ptr<Expr> > stmts);

  void accept(ExprVisitor &visitor) override;
};

class NumberExpr : public Expr
{
public:
  int value;

  explicit NumberExpr(int val);

  void accept(ExprVisitor &visitor) override;
};

class StringExpr : public Expr
{
public:
  std::string value;

  explicit StringExpr(std::string val);

  void accept(ExprVisitor &visitor) override;
};

class BinaryExpr : public Expr
{
public:
  std::unique_ptr<Expr> left;

  Token operation;

  std::unique_ptr<Expr> right;

  BinaryExpr(std::unique_ptr<Expr> l, Token oper, std::unique_ptr<Expr> r);

  void accept(ExprVisitor &visitor) override;
};

class ConcatExpr : public Expr
{
public:
  std::unique_ptr<Expr> left;

  std::unique_ptr<Expr> right;

  ConcatExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r);

  void accept(ExprVisitor &visitor) override;
};

class AssignExpr : public Expr
{
public:
  std::string name;

  std::unique_ptr<Expr> value;

  AssignExpr(std::string m, std::unique_ptr<Expr> v);

  void accept(ExprVisitor &visitor) override;
};

class VariableExpr : public Expr
{
public:
  std::string name;

  explicit VariableExpr(std::string);

  void accept(ExprVisitor &visitor) override;
};

class LetExpr : public Expr
{
public:
  std::string name;

  std::unique_ptr<Expr> initialiser;

  LetExpr(std::string n, std::unique_ptr<Expr> init);

  void accept(ExprVisitor &visitor) override;
};

class CallExpr : public Expr
{
public:
  std::string function_name;

  std::vector<std::unique_ptr<Expr> > arguments;

  CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args);

  void accept(ExprVisitor &visitor) override;
};
