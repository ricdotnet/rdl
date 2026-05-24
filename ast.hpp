#pragma once

#include "token.hpp"

#include <memory>
#include <string>
#include <vector>

class Expr {
public:
    virtual ~Expr() = default;
};

class NumberExpr : public Expr {
public:
    int value;

    explicit NumberExpr(int val);
};

class StringExpr : public Expr {
public:
    std::string value;

    explicit StringExpr(std::string val);
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token operation;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> l, Token oper, std::unique_ptr<Expr> r);
};

class AssignExpr : public Expr {
public:
    std::string name;
    std::unique_ptr<Expr> value;

    AssignExpr(std::string m, std::unique_ptr<Expr> v);
};

class VariableExpr : public Expr {
public:
    std::string name;

    explicit VariableExpr(std::string);
};

class CallExpr : public Expr {
public:
    std::string function_name;
    std::vector<std::unique_ptr<Expr> > arguments;

    CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args);
};
