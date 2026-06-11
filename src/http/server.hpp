#pragma once

#include "../runtime_context.hpp"

class HttpServer
{
private:
  int status_code = 200;

  std::unordered_map<std::string, std::string> headers;

  static void register_request_methods(std::unordered_map<std::string, Value> *properties);

  void register_response_methods(std::unordered_map<std::string, Value> *properties);

  static void process_headers(std::istream &request, std::unordered_map<std::string, Value> *headers_map);

  static void process_body(std::istream &request, std::string &body);

public:
  [[noreturn]] void start_server(int port, RuntimeContext context);

  void handle_client(int client, RuntimeContext context);
};
