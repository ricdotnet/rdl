#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "./token.hpp"
#include "./visitor.hpp"

class Expr
{
public:
  virtual ~Expr() = default;

  virtual void accept(ExprVisitor &visitor) = 0;
};

class FunctionExpr : public Expr
{
public:
  std::string name;

  std::vector<std::string> parameters;

  std::unique_ptr<BlockStmt> body;

  std::optional<std::string> receiver_type;

  explicit FunctionExpr(std::string name, std::vector<std::string> parameters, std::unique_ptr<BlockStmt> body,
                        std::optional<std::string> receiver_type = std::nullopt);

  void accept(ExprVisitor &visitor) override;
};

class ReturnStmt : public Expr
{
public:
  std::unique_ptr<Expr> value;

  explicit ReturnStmt(std::unique_ptr<Expr> v);

  void accept(ExprVisitor &visitor) override;
};

// TODO: bodies should be BlockStmt
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

class WhileExpr : public Expr
{
public:
  std::unique_ptr<Expr> condition;

  std::unique_ptr<BlockStmt> body;

  explicit WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> body);

  void accept(ExprVisitor &visitor) override;
};

class ForStmt : public Expr
{
public:
  std::string iterator;

  // Can be RangeExpr or ArrayExpr in the future
  std::unique_ptr<Expr> iterable;

  std::unique_ptr<BlockStmt> body;

  explicit ForStmt(std::string i, std::unique_ptr<Expr> iter, std::unique_ptr<BlockStmt> body);

  void accept(ExprVisitor &visitor) override;
};

class RangeExpr : public Expr
{
public:
  int start;

  int end;

  int step;

  explicit RangeExpr(int start, int end, int step);

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

class BooleanExpr : public Expr
{
public:
  bool value;

  explicit BooleanExpr(bool val);

  void accept(ExprVisitor &visitor) override;
};

class BinaryExpr : public Expr
{
public:
  std::unique_ptr<Expr> left;

  Token operation;

  std::unique_ptr<Expr> right;

  explicit BinaryExpr(std::unique_ptr<Expr> l, Token oper, std::unique_ptr<Expr> r);

  void accept(ExprVisitor &visitor) override;
};

class UnaryExpr : public Expr
{
public:
  Token operation;

  std::unique_ptr<Expr> operand;

  explicit UnaryExpr(Token op, std::unique_ptr<Expr> operand);

  void accept(ExprVisitor &visitor) override;
};

class ConcatExpr : public Expr
{
public:
  std::unique_ptr<Expr> left;

  std::unique_ptr<Expr> right;

  explicit ConcatExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r);

  void accept(ExprVisitor &visitor) override;
};

class AssignExpr : public Expr
{
public:
  std::string name;

  std::unique_ptr<Expr> value;

  explicit AssignExpr(std::string n, std::unique_ptr<Expr> v);

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

  explicit LetExpr(std::string n, std::unique_ptr<Expr> init);

  void accept(ExprVisitor &visitor) override;
};

class CallExpr : public Expr
{
public:
  std::string function_name;

  std::vector<std::unique_ptr<Expr> > arguments;

  explicit CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args);

  void accept(ExprVisitor &visitor) override;
};

class MethodCallExpr : public Expr
{
public:
  std::unique_ptr<Expr> receiver;

  std::string method_name;

  std::vector<std::unique_ptr<Expr> > arguments;

  explicit MethodCallExpr(std::unique_ptr<Expr> recv, std::string method, std::vector<std::unique_ptr<Expr> > args);

  void accept(ExprVisitor &visitor) override;
};

class ObjectExpr : public Expr
{
public:
  std::unordered_map<std::string, std::unique_ptr<Expr> > fields;

  explicit ObjectExpr(std::unordered_map<std::string, std::unique_ptr<Expr> > fields);

  void accept(ExprVisitor &visitor) override;
};

class DotExpr : public Expr
{
public:
  std::string field_name;

  std::unique_ptr<Expr> receiver;

  explicit DotExpr(std::string field_name, std::unique_ptr<Expr> recv);

  void accept(ExprVisitor &visitor) override;
};
