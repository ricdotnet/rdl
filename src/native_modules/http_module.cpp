#include <vector>
#include "./native_module.hpp"
#include "../http/server.hpp"

class HttpModule : NativeModule
{
private:
  Environment *environment;

public:
  explicit HttpModule(Environment *environment) : NativeModule(), environment(environment) {}

  [[nodiscard]] std::string name() const override { return "http"; }

  [[nodiscard]] Value init() override
  {
    const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

    (*functions)["listen"] = Value::builtin_function_value([this](const Value &, const std::vector<Value> &args) {
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
      http_server.start_server(port, this->environment);

      return Value::nil_value();
    });

    return Value::object_value(functions);
  }
};
