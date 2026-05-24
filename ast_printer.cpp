#include <iostream>

#include "ast.hpp"

class ASTPrinter : public ExprVisitor {
    int indent = 0;

    void print_indent() const {
        for (int i = 0; i < indent; i++) {
            std::cout << "  ";
        }
    }

public:
    void visit(NumberExpr &expr) override {
        print_indent();
        std::cout << "Number(" << expr.value << ")\n";
    }

    void visit(StringExpr &expr) override {
        print_indent();
        std::cout << "String(" << expr.value << ")\n";
    }

    void visit(VariableExpr &expr) override {
        print_indent();
        std::cout << "Variable(" << expr.name << ")\n";
    }

    void visit(AssignExpr &expr) override {
        print_indent();
        std::cout << "Assign(" << expr.name << ")\n";

        indent++;
        expr.value->accept(*this);
        indent--;
    }

    void visit(BinaryExpr &expr) override {
        print_indent();
        std::cout << "Binary(" << expr.operation.value << ")\n";

        indent++;
        expr.left->accept(*this);
        expr.right->accept(*this);
        indent--;
    }

    void visit(ConcatExpr &expr) override {
        print_indent();
        std::cout << "Concat(";

        indent++;
        expr.left->accept(*this);
        expr.right->accept(*this);
        indent--;
    }

    void visit(CallExpr &expr) override {
        print_indent();
        std::cout << "Call(" << expr.function_name << ")\n";
        std::cout << std::endl;

        indent++;
        for (const auto &arg: expr.arguments) {
            arg->accept(*this);
        }
        indent--;
    }
};
