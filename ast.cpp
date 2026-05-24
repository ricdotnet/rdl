#include "ast.hpp"

#include <utility>

NumberExpr::NumberExpr(int val) : value(val) {
}

void NumberExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

StringExpr::StringExpr(std::string val) : value(std::move(val)) {
}

void StringExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> l, Token oper,
                       std::unique_ptr<Expr> r)
    : left(std::move(l)), operation(oper), right(std::move(r)) {
}

void BinaryExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AssignExpr::AssignExpr(std::string n, std::unique_ptr<Expr> (v))
    : name(std::move(n)), value(std::move(v)) {
}

void AssignExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

VariableExpr::VariableExpr(std::string n) : name(std::move(n)) {
}

void VariableExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

CallExpr::CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args) : function_name(std::move(func)),
    arguments(std::move(args)) {
}

void CallExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}