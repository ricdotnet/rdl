#include "./http_lambdas.hpp"
#include "../http/http_server.cpp"

NativeFn listen(RuntimeContext context)
{
  return [context](const Value &, const std::vector<Value> &args) {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for listen method.", "Found " + std::to_string(args.size()));
    }

    if (args[0].type != ValueType::Number && args[0].type != ValueType::String)
    {
      ErrorService::runtime_error("Expected number or string for port argument.", "");
    }

    int port = -1;
    if (args[0].is_number())
    {
      if (args[0].number < 0 || args[0].number > 65535)
      {
        ErrorService::runtime_error("Port number must be between 0 and 65535.", "");
      }

      port = args[0].number;
    }

    if (args[0].is_string())
    {
      port = std::stoi(args[0].string);
    }

    HttpServer http_server;
    http_server.listen(port, context);

    return Value::nil_value();
  };
}
