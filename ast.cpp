#include "ast.hpp"

#include <utility>

NumberExpr::NumberExpr(int val) : value(val) {
}

StringExpr::StringExpr(std::string val) : value(std::move(val)) {
}

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> l, Token oper,
                       std::unique_ptr<Expr> r)
    : left(std::move(l)), operation(oper), right(std::move(r)) {
}

AssignExpr::AssignExpr(std::string n, std::unique_ptr<Expr> (v))
    : name(std::move(n)), value(std::move(v)) {
}

VariableExpr::VariableExpr(std::string n) : name(std::move(n)) {
}

CallExpr::CallExpr(std::string func, std::vector<std::unique_ptr<Expr> > args) : function_name(std::move(func)),
    arguments(std::move(args)) {
}
