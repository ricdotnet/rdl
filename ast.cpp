#include "ast.hpp"
#include <utility>

IfStmt::IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> then, std::unique_ptr<Expr> else_branch)
  : condition(std::move(cond)), then_branch(std::move(then)), else_branch(std::move(else_branch)) {}

void IfStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Expr> > stmts) : statements(std::move(stmts)) {}

void BlockStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

NumberExpr::NumberExpr(const int val) : value(val) {}

void NumberExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

StringExpr::StringExpr(std::string val) : value(std::move(val)) {}

void StringExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> l, Token oper, std::unique_ptr<Expr> r)
  : left(std::move(l)), operation(oper), right(std::move(r)) {}

void BinaryExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

ConcatExpr::ConcatExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
  : left(std::move(l)), right(std::move(r)) {}

void ConcatExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

AssignExpr::AssignExpr(std::string n, std::unique_ptr<Expr> (v))
  : name(std::move(n)), value(std::move(v)) {}

void AssignExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

VariableExpr::VariableExpr(std::string n) : name(std::move(n)) {}

void VariableExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

LetExpr::LetExpr(std::string n, std::unique_ptr<Expr> init)
  : name(std::move(n)), initialiser(std::move(init)) {}

void LetExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

CallExpr::CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args)
  : function_name(std::move(func)), arguments(std::move(args)) {}

void CallExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }
