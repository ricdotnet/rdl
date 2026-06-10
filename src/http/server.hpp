#pragma once

#include "../environment.hpp"

class HttpServer
{
private:
  int status_code = 200;

  std::unordered_map<std::string, std::string> headers;

  void register_response_methods(std::unordered_map<std::string, Value> *properties);

public:
  [[noreturn]] void start_server(int port, Environment *environment);

  void handle_client(int client, Environment *environment);
};
