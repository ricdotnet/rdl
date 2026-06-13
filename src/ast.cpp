#include "./ast.hpp"
#include <utility>

FunctionExpr::FunctionExpr(std::string name, std::vector<std::string> parameters, std::unique_ptr<BlockStmt> body,
                           std::optional<std::string> receiver_type)
  : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)),
    receiver_type(std::move(receiver_type)) {}

void FunctionExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> v) : value(std::move(v)) {}

void ReturnStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

IfStmt::IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Expr> then, std::unique_ptr<Expr> else_branch)
  : condition(std::move(cond)), then_branch(std::move(then)), else_branch(std::move(else_branch)) {}

void IfStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Expr> > stmts) : statements(std::move(stmts)) {}

void BlockStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

StructStmt::StructStmt(std::string name, std::unordered_map<std::string, StructDefinitionField> fields)
  : name(std::move(name)), fields(std::move(fields)) {}

void StructStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

WhileExpr::WhileExpr(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> body)
  : condition(std::move(cond)), body(std::move(body)) {}

ForStmt::ForStmt(std::string i, std::optional<std::string> index_name, std::unique_ptr<Expr> iter,
                 std::unique_ptr<BlockStmt> body) : iterator(std::move(i)), index_name(std::move(index_name)),
                                                    iterable(std::move(iter)), body(std::move(body))
{}

void ForStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

RangeExpr::RangeExpr(const int start, const int end, const int step, const bool inclusive) : start(start), end(end),
  step(step), inclusive(inclusive)
{}

void RangeExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

void WhileExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

NumberExpr::NumberExpr(const int val) : value(val) {}

void NumberExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

StringExpr::StringExpr(std::string val) : value(std::move(val)) {}

void StringExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

BooleanExpr::BooleanExpr(const bool val) : value(val) {}

void BooleanExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> l, Token oper, std::unique_ptr<Expr> r)
  : left(std::move(l)), operation(std::move(oper)), right(std::move(r)) {}

void BinaryExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

UnaryExpr::UnaryExpr(Token op, std::unique_ptr<Expr> operand)
  : operation(std::move(op)), operand(std::move(operand)) {}

void UnaryExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

ConcatExpr::ConcatExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
  : left(std::move(l)), right(std::move(r)) {}

void ConcatExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

AssignExpr::AssignExpr(std::unique_ptr<Expr> l, std::unique_ptr<Expr> v)
  : left(std::move(l)), value(std::move(v)) {}

void AssignExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

VariableExpr::VariableExpr(std::string n) : name(std::move(n)) {}

void VariableExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

LetExpr::LetExpr(std::string n, std::unique_ptr<Expr> init)
  : name(std::move(n)), initialiser(std::move(init)) {}

void LetExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

CallExpr::CallExpr(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr> > args)
  : callee(std::move(callee)), arguments(std::move(args)) {}

void CallExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

MethodCallExpr::MethodCallExpr(std::unique_ptr<Expr> recv, std::string method, std::vector<std::unique_ptr<Expr> > args)
  : receiver(std::move(recv)), method_name(std::move(method)), arguments(std::move(args)) {}

void MethodCallExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

ObjectExpr::ObjectExpr(std::unordered_map<std::string, std::unique_ptr<Expr> > fields)
  : fields(std::move(fields)) {}

void ObjectExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

DotExpr::DotExpr(std::string field_name, std::unique_ptr<Expr> recv)
  : field_name(std::move(field_name)), receiver(std::move(recv)) {}

void DotExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

ArrayExpr::ArrayExpr(const ValueType declared_type, std::vector<std::unique_ptr<Expr> > elements, std::string type_name)
  : declared_type(declared_type), elements(std::move(elements)), type_name(std::move(type_name)) {}

void ArrayExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

IndexExpr::IndexExpr(std::unique_ptr<Expr> receiver_array, std::unique_ptr<Expr> index)
  : receiver_array(std::move(receiver_array)), index(std::move(index)) {}

void IndexExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

StructInitExpr::StructInitExpr(std::string type_name, std::unordered_map<std::string, std::unique_ptr<Expr> > fields)
  : type_name(std::move(type_name)), fields(std::move(fields)) {}

void StructInitExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

ImportExpr::ImportExpr(std::string module_name)
  : module_name(std::move(module_name)) {}

void ImportExpr::accept(ExprVisitor &visitor) { visitor.visit(*this); }

GroupStmt::GroupStmt(std::string path, std::vector<std::unique_ptr<Expr> > routes)
  : path(std::move(path)), routes(std::move(routes)) {}

void GroupStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }

RouteStmt::RouteStmt(std::string method, std::string path, std::unique_ptr<BlockStmt> body,
                     std::optional<std::string> req_identifier, std::optional<std::string> res_identifier)
  : method(std::move(method)), path(std::move(path)), body(std::move(body)), req_identifier(std::move(req_identifier)),
    res_identifier(std::move(res_identifier)) {}

void RouteStmt::accept(ExprVisitor &visitor) { visitor.visit(*this); }
