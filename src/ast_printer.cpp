#include <iostream>
#include "./ast.hpp"

class ASTPrinter : public ExprVisitor
{
  int indent = 0;

  void print_indent() const
  {
    for (int i = 0; i < indent; i++)
    {
      std::cout << "  ";
    }
  }

public:
  void visit(FunctionExpr &expr) override
  {
    print_indent();
    std::cout << "Function(" << expr.name << ")\n";
  }

  void visit(ReturnStmt &stmt) override
  {
    print_indent();
    std::cout << "Return(" << stmt.value << ")\n";
  }

  void visit(IfStmt &stmt) override
  {
    print_indent();
    std::cout << "IfStmt(\n";
    indent++;
    stmt.condition->accept(*this);
    stmt.then_branch->accept(*this);
    if (stmt.else_branch)
    {
      stmt.else_branch->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(BlockStmt &stmt) override
  {
    print_indent();
    std::cout << "BlockStmt(\n";
    indent++;
    for (const auto &expr: stmt.statements)
    {
      expr->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(StructStmt &expr) override
  {
    print_indent();
    std::cout << "StructStmt(\n";
    indent++;
    for (const auto &[field_name, field_type]: expr.fields)
    {
      print_indent();
      std::cout << field_name << ": ";
      std::cout << Value::type_name(field_type) << "\n";
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(WhileExpr &expr) override
  {
    print_indent();
    std::cout << "WhileExpr(\n";
    indent++;
    expr.condition->accept(*this);
    expr.body->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(ForStmt &expr) override
  {
    print_indent();
    std::cout << "ForStmt(\n";
    indent++;
    expr.iterable->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(RangeExpr &expr) override
  {
    print_indent();
    std::cout << "RangeExpr(\n";
    indent++;
    expr.accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(NumberExpr &expr) override
  {
    print_indent();
    std::cout << "Number(" << expr.value << ")\n";
  }

  void visit(StringExpr &expr) override
  {
    print_indent();
    std::cout << "String(" << expr.value << ")\n";
  }

  void visit(BooleanExpr &expr) override
  {
    print_indent();
    std::cout << "Boolean(" << expr.value << ")\n";
  }

  void visit(BinaryExpr &expr) override
  {
    print_indent();
    std::cout << "Binary(" << expr.operation.value << ")\n";

    indent++;
    expr.left->accept(*this);
    expr.right->accept(*this);
    indent--;
  }

  void visit(UnaryExpr &expr) override
  {
    print_indent();
    std::cout << "Unary(" << expr.operation.value << ")\n";
  }

  void visit(ConcatExpr &expr) override
  {
    print_indent();
    std::cout << "Concat(";

    indent++;
    expr.left->accept(*this);
    expr.right->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(VariableExpr &expr) override
  {
    print_indent();
    std::cout << "Variable(" << expr.name << ")\n";
  }

  void visit(AssignExpr &expr) override
  {
    print_indent();
    std::cout << "Assign(\n";

    indent++;
    expr.left->accept(*this);
    expr.value->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(LetExpr &expr) override
  {
    print_indent();
    std::cout << "Let(" << expr.name << ")\n";

    indent++;
    expr.initialiser->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(CallExpr &expr) override
  {
    print_indent();
    std::cout << "Call(\n";
    expr.callee->accept(*this);
    indent++;
    for (const auto &arg: expr.arguments)
    {
      arg->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(MethodCallExpr &expr) override
  {
    print_indent();
    std::cout << "MethodCall(\n";
    expr.receiver->accept(*this);
    indent++;
    for (const auto &arg: expr.arguments)
    {
      arg->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(ObjectExpr &expr) override
  {
    print_indent();
    std::cout << "Object(\n";
    indent++;
    for (const auto &[field_name, field_expr]: expr.fields)
    {
      print_indent();
      std::cout << "Identifier(" << field_name << "):";
      field_expr->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(DotExpr &expr) override
  {
    print_indent();
    std::cout << "DotExpr(" << expr.field_name << ")\n";
  }

  void visit(ArrayExpr &expr) override
  {
    print_indent();
    std::cout << "Array(\n";
    indent++;
    for (const auto &exp: expr.elements)
    {
      exp->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(IndexExpr &expr) override
  {
    print_indent();
    std::cout << "IndexExpr(\n";
    indent++;
    expr.receiver_array->accept(*this);
    expr.index->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }
};
