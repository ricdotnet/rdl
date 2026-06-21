#pragma once

#include <vector>
#include "../ast.hpp"

struct RegisteredRoute
{
  std::string method;

  std::string path;

  BlockStmt *body;

  std::optional<std::string> req_identifier;

  std::optional<std::string> res_identifier;
};

class Router
{
private:
  std::vector<RegisteredRoute> routes;

public:
  void register_route(const std::string &method, const std::string &path, BlockStmt *body,
                      const std::optional<std::string> &req_identifier,
                      const std::optional<std::string> &res_identifier);

  [[nodiscard]] RegisteredRoute find_route(const std::string &method, const std::string &path) const;
};