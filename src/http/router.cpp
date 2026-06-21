#include "./router.hpp"

void Router::register_route(const std::string &method, const std::string &path, BlockStmt *body,
                            const std::optional<std::string> &req_identifier,
                            const std::optional<std::string> &res_identifier)
{
  routes.push_back({method, path, body, req_identifier, res_identifier});
}

RegisteredRoute Router::find_route(const std::string &method, const std::string &path) const
{
  for (const auto &route: routes)
  {
    if (route.method == method && route.path == path)
    {
      return route;
    }
  }

  return {};
}