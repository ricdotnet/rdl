#pragma once

class FunctionExpr;

class IfStmt;

class BlockStmt;

class WhileExpr;

class NumberExpr;

class StringExpr;

class BinaryExpr;

class ConcatExpr;

class VariableExpr;

class AssignExpr;

class LetExpr;

class CallExpr;

class ExprVisitor
{
public:
  virtual ~ExprVisitor() = default;

  virtual void visit(FunctionExpr &expr) = 0;

  virtual void visit(IfStmt &expr) = 0;

  virtual void visit(BlockStmt &expr) = 0;

  virtual void visit(WhileExpr &expr) = 0;

  virtual void visit(NumberExpr &expr) = 0;

  virtual void visit(StringExpr &expr) = 0;

  virtual void visit(BinaryExpr &expr) = 0;

  virtual void visit(ConcatExpr &expr) = 0;

  virtual void visit(VariableExpr &expr) = 0;

  virtual void visit(AssignExpr &expr) = 0;

  virtual void visit(LetExpr &expr) = 0;

  virtual void visit(CallExpr &expr) = 0;
};
