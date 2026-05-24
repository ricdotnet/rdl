#include "ast_printer.hpp"

#include <iostream>

void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        std::cout << "  ";
    }
}

void print_ast(Expr *expr, int indent) {
    if (auto num = dynamic_cast<NumberExpr *>(expr)) {
        print_indent(indent);
        std::cout << "Number(" << num->value << ")\n";
        return;
    }

    if (auto str = dynamic_cast<StringExpr *>(expr)) {
        print_indent(indent);
        std::cout << "String(\"" << str->value << "\")\n";
        return;
    }

    if (auto var = dynamic_cast<VariableExpr *>(expr)) {
        print_indent(indent);
        std::cout << "Variable(" << var->name << ")\n";
        return;
    }

    if (auto assign = dynamic_cast<AssignExpr *>(expr)) {
        print_indent(indent);
        std::cout << "Assign(" << assign->name << ")\n";

        print_ast(assign->value.get(), indent + 1);
        return;
    }

    if (auto binary = dynamic_cast<BinaryExpr *>(expr)) {
        print_indent(indent);

        std::cout
                << "Binary("
                << binary->operation.value
                << ")\n";

        print_ast(binary->left.get(), indent + 1);
        print_ast(binary->right.get(), indent + 1);

        return;
    }

    if (auto call = dynamic_cast<CallExpr *>(expr)) {
        print_indent(indent);

        std::cout
                << "Call("
                << call->function_name
                << ")\n";

        for (auto &arg: call->arguments) {
            print_ast(arg.get(), indent + 1);
        }

        return;
    }

    print_indent(indent);
    std::cout << "Unknown\n";
}
