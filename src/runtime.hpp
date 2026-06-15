#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include "./ast.hpp"
#include "./interpreter.hpp"

struct RegisteredRoute
{
  std::string method;

  std::string path;

  BlockStmt *body;

  std::optional<std::string> req_identifier;

  std::optional<std::string> res_identifier;
};

class Runtime
{
public:
  Runtime() : out(&std::cout) {}

  std::ostream *out;

  std::unordered_map<std::string, Value> globals;

  std::vector<RegisteredRoute> routes;

  void init_type_methods();

  void add_global(const std::string &name, const Value &value);

  Value resolve(const std::string &name);

  void register_route(const std::string &method, const std::string &path, BlockStmt *body,
                      const std::optional<std::string> &req_identifier,
                      const std::optional<std::string> &res_identifier);

  RegisteredRoute find_route(const std::string &method, const std::string &path) const;
};
