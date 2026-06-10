#include "./server.hpp"
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../interpreter.cpp"


[[noreturn]] void HttpServer::start_server(const int port, Environment *environment)
{
  const int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  constexpr int yes = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

  ::listen(server_fd, 10);

  const auto runtime = environment->get_runtime();
  *runtime->out << "Server is listening on port " << port << '\n';

  while (true)
  {
    const int client = accept(server_fd, nullptr, nullptr);

    handle_client(client, environment);

    close(client);
  }
}

void HttpServer::handle_client(const int client, Environment *environment)
{
  char buffer[4096];

  const ssize_t received = recv(client, buffer, sizeof(buffer) - 1, 0);

  if (received <= 0)
  {
    return;
  }

  buffer[received] = '\0';

  std::istringstream request(buffer);

  std::string method;
  std::string path;
  std::string version;

  request >> method >> path >> version;

  auto request_handle = std::make_shared<RequestHandle>();
  request_handle->client_fd = client;

  // TODO: implement a simple dictionary of key value strings
  auto request_headers = std::make_shared<std::unordered_map<std::string, Value> >();
  auto request_props = std::make_shared<std::unordered_map<std::string, Value> >();

  // TODO: refactor info helper
  std::string line;
  std::getline(request, line);
  while (std::getline(request, line))
  {
    if (line == "\r" || line.empty())
    {
      break;
    }

    auto pos = line.find(':');

    if (pos == std::string::npos)
    {
      continue;
    }

    auto key = line.substr(0, pos);

    auto value = line.substr(pos + 1);

    if (!value.empty() && value.front() == ' ')
    {
      value.erase(0, 1);
    }

    if (!value.empty() && value.back() == '\r')
    {
      value.pop_back();
    }

    (*request_headers)[key] = Value::string_value(value);
  }

  request_props->insert({"headers", Value::object_value(request_headers)});
  request_props->insert({"method", Value::string_value(method)});
  request_props->insert({"path", Value::string_value(path)});

  auto request_object = Value::object_value(request_props);
  request_object.object.native_object = request_handle;

  const auto route = environment->get_runtime()->find_route(method, path);

  if (!route)
  {
    const auto response = "HTTP/1.1 404 Not Found\r\n" "Content-Length: 9\r\n" "\r\n" "Not Found";

    send(client, response, strlen(response), 0);

    return;
  }

  auto response_handle = std::make_shared<ResponseHandle>();
  response_handle->client_fd = client;

  auto response_props = std::make_shared<std::unordered_map<std::string, Value> >();
  auto response_object = Value::object_value(response_props);

  response_object.object.native_object = response_handle;

  register_request_methods(request_props.get());
  register_response_methods(response_props.get());

  Interpreter interpreter(environment);
  interpreter.execute_route(*route, request_object, response_object);

  close(client);
}

void HttpServer::register_request_methods(std::unordered_map<std::string, Value> *properties)
{
  (*properties)["get_header"] = Value::builtin_function_value(
    [](const Value &receiver, const std::vector<Value> &args) -> Value {
      if (args.empty() || args.size() > 1)
      {
        ErrorService::runtime_error("Expected 1 argument for get_header method.",
                                    "Found " + std::to_string(args.size()));
      }

      if (args[0].type != ValueType::String)
      {
        ErrorService::runtime_error("Expected string argument for get_header method.",
                                    "Found " + Value::type_name(args[0].type));
      }

      const auto header = args[0].string;
      const auto request_props = receiver.object.properties;
      auto &headers = request_props->at("headers").object;

      if (headers.properties->contains(header))
      {
        return headers.properties->at(header);
      }

      return Value::undefined_value();
    });
}

void HttpServer::register_response_methods(std::unordered_map<std::string, Value> *properties)
{
  (*properties)["send"] = Value::builtin_function_value(
    [this](const Value &receiver, const std::vector<Value> &args) -> Value {
      const auto response = std::static_pointer_cast<ResponseHandle>(receiver.object.native_object);

      const std::string body = args.empty() ? "" : args[0].to_string();

      std::ostringstream out;

      out << "HTTP/1.1 " << status_code << " OK\r\n" << "Content-Type: text/plain\r\n" << "Content-Length: " << body.
          size() << "\r\n";

      for (const auto &[header, value]: headers)
      {
        out << header << ": " << value << "\r\n";
      }

      out << "\r\n" << body;

      send(response->client_fd, out.str().c_str(), out.str().size(), 0);

      return Value::nil_value();
    });

  (*properties)["status_code"] = Value::builtin_function_value(
    [this](const Value &, const std::vector<Value> &args) -> Value {
      if (args.empty())
      {
        ErrorService::runtime_error("Expected 1 argument for status method.", "Found 0");
      }

      this->status_code = args[0].number;

      return Value::nil_value();
    });

  (*properties)["write_header"] = Value::builtin_function_value(
    [this](const Value &, const std::vector<Value> &args) -> Value {
      if (args.size() != 2)
      {
        ErrorService::runtime_error("Expected 2 arguments for set_header method.",
                                    "Found " + std::to_string(args.size()));
      }

      this->headers.insert({args[0].string, args[1].string});

      return Value::nil_value();
    });
}
