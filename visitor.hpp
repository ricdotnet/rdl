#pragma once

class NumberExpr;
class StringExpr;
class BinaryExpr;
class VariableExpr;
class AssignExpr;
class CallExpr;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;

    virtual void visit(NumberExpr& expr) = 0;
    virtual void visit(StringExpr& expr) = 0;
    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(VariableExpr& expr) = 0;
    virtual void visit(AssignExpr& expr) = 0;
    virtual void visit(CallExpr& expr) = 0;
};