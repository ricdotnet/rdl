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
    std::cout << "Function(\n";
    indent++;
    expr.body->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(ReturnStmt &stmt) override
  {
    print_indent();
    std::cout << "Return\n";
    indent++;
    stmt.value->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
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
    std::cout << "StructStmt(" + expr.name + " \n";
    indent++;
    for (const auto &[field_name, field_def]: expr.fields)
    {
      print_indent();

      std::cout << field_name << ": ";
      std::cout << Value::type_name(field_def.type.type) << "\n";
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
    std::cout << "RangeExpr(" << expr.start << " to " << expr.end << " by " << expr.step << ")\n";
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
    std::cout << "Binary(\n";
    indent++;
    expr.left->accept(*this);
    print_indent();
    std::cout << token_type_to_string(expr.operation.type) << "(" << expr.operation.value << ")" << "\n";
    expr.right->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(UnaryExpr &expr) override
  {
    print_indent();
    std::cout << "Unary(" << expr.operation.value << ")\n";
  }

  void visit(ConcatExpr &expr) override
  {
    print_indent();
    std::cout << "Concat(\n";
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
    indent++;
    expr.callee->accept(*this);
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
    indent++;
    expr.receiver->accept(*this);
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
      std::cout << "Identifier(" << field_name << "\n";
      indent++;
      field_expr->accept(*this);
      indent--;
      print_indent();
      std::cout << ")\n";
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(DotExpr &expr) override
  {
    print_indent();
    std::cout << "DotExpr(\n";
    indent++;
    expr.receiver->accept(*this);
    print_indent();
    std::cout << "Identifier(" << expr.field_name << ")\n";
    indent--;
    print_indent();
    std::cout << ")\n";
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

  void visit(StructInitExpr &expr) override
  {
    print_indent();
    std::cout << "StructInitExpr(" + expr.type_name + "\n";
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

  void visit(ImportExpr &expr) override
  {
    print_indent();
    std::cout << "Import(" << expr.module_name << ")\n";
  }

  void visit(GroupStmt &expr) override
  {
    print_indent();
    std::cout << "GroupStmt(\n";
    indent++;
    print_indent();
    std::cout << "Prefix(" << expr.path << ")\n";
    for (const auto &route: expr.routes)
    {
      route->accept(*this);
    }
    indent--;
    print_indent();
    std::cout << ")\n";
  }

  void visit(RouteStmt &expr) override
  {
    print_indent();
    std::cout << "RouteStmt(\n";
    indent++;
    print_indent();
    std::cout << "Method(" << expr.method << ")\n";
    print_indent();
    std::cout << "Path(" << expr.path << ")\n";
    print_indent();
    std::cout << "Body(\n";
    indent++;
    expr.body->accept(*this);
    indent--;
    print_indent();
    std::cout << ")\n";
    indent--;
    print_indent();
    std::cout << ")\n";
  }
};
