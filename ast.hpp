#pragma once

#include "token.hpp"

#include <memory>
#include <string>
#include <vector>

#include "visitor.hpp"

class Expr {
public:
    virtual ~Expr() = default;

    virtual void accept(ExprVisitor& visitor) = 0;
};

class NumberExpr : public Expr {
public:
    int value;

    explicit NumberExpr(int val);

    void accept(ExprVisitor& visitor) override;
};

class StringExpr : public Expr {
public:
    std::string value;

    explicit StringExpr(std::string val);

    void accept(ExprVisitor& visitor) override;
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token operation;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> l, Token oper, std::unique_ptr<Expr> r);

    void accept(ExprVisitor& visitor) override;
};

class ConcatExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    ConcatExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r);

    void accept(ExprVisitor& visitor) override;
};

class AssignExpr : public Expr {
public:
    std::string name;
    std::unique_ptr<Expr> value;

    AssignExpr(std::string m, std::unique_ptr<Expr> v);

    void accept(ExprVisitor& visitor) override;
};

class VariableExpr : public Expr {
public:
    std::string name;

    explicit VariableExpr(std::string);

    void accept(ExprVisitor& visitor) override;
};

class CallExpr : public Expr {
public:
    std::string function_name;
    std::vector<std::unique_ptr<Expr> > arguments;

    CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args);

    void accept(ExprVisitor& visitor) override;
};
