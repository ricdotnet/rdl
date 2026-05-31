#pragma once

class FunctionExpr;

class ReturnStmt;

class IfStmt;

class BlockStmt;

class WhileExpr;

class ForStmt;

class RangeExpr;

class NumberExpr;

class StringExpr;

class BooleanExpr;

class BinaryExpr;

class UnaryExpr;

class ConcatExpr;

class VariableExpr;

class AssignExpr;

class LetExpr;

class CallExpr;

class MethodCallExpr;

class ObjectExpr;

class ObjectAssignExpr;

class DotExpr;

class ExprVisitor
{
public:
  virtual ~ExprVisitor() = default;

  virtual void visit(FunctionExpr &expr) = 0;

  virtual void visit(ReturnStmt &expr) = 0;

  virtual void visit(IfStmt &expr) = 0;

  virtual void visit(BlockStmt &expr) = 0;

  virtual void visit(WhileExpr &expr) = 0;

  virtual void visit(ForStmt &expr) = 0;

  virtual void visit(RangeExpr &expr) = 0;

  virtual void visit(NumberExpr &expr) = 0;

  virtual void visit(StringExpr &expr) = 0;

  virtual void visit(BooleanExpr &expr) = 0;

  virtual void visit(BinaryExpr &expr) = 0;

  virtual void visit(UnaryExpr &expr) = 0;

  virtual void visit(ConcatExpr &expr) = 0;

  virtual void visit(VariableExpr &expr) = 0;

  virtual void visit(AssignExpr &expr) = 0;

  virtual void visit(LetExpr &expr) = 0;

  virtual void visit(CallExpr &expr) = 0;

  virtual void visit(MethodCallExpr &expr) = 0;

  virtual void visit(ObjectExpr &expr) = 0;

  virtual void visit(ObjectAssignExpr &expr) = 0;

  virtual void visit(DotExpr &expr) = 0;
};
