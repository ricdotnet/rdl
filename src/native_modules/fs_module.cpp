#include <fstream>
#include <sstream>
#include "native_module.hpp"

class FileSystemModule : NativeModule
{
public:
  explicit FileSystemModule() : NativeModule() {}

  [[nodiscard]] std::string name() const override { return "fs"; }

  [[nodiscard]] Value init() override
  {
    const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

    (*functions)["read_file"] = Value::builtin_function_value([](const Value &, const std::vector<Value> &args) {
      return read_file(args);
    });

    return Value::object_value(functions);
  }

private:
  static Value read_file(const std::vector<Value> &args)
  {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for read_file, found ", std::to_string(args.size()));
    }

    const std::string path = args[0].string;

    std::ifstream file(path);

    if (!file.is_open())
    {
      ErrorService::runtime_error("Could not open file", path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return Value::string_value(buffer.str());
  }
};
