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
    (*functions)["write_file"] = Value::builtin_function_value([](const Value &, const std::vector<Value> &args) {
      return write_file(args);
    });
    (*functions)["open_file"] = Value::builtin_function_value([](const Value &, const std::vector<Value> &args) {
      return open_file(args);
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

  static Value write_file(const std::vector<Value> &)
  {
    ErrorService::runtime_error("Not implemented", "");
    return Value::nil_value();
  }

  static Value open_file(const std::vector<Value> &args)
  {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for read_file, found ", std::to_string(args.size()));
    }

    const std::string path = args[0].string;

    const auto native_object = std::make_shared<FileHandler>();
    native_object->stream.open(path);

    const auto properties = std::make_shared<std::unordered_map<std::string, Value> >();

    properties.get()->insert({
      "read_line", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = std::static_pointer_cast<FileHandler>(receiver.object.native_object);

        if (!file)
        {
          ErrorService::runtime_error("Invalid file handle", "");
        }

        std::string line;

        if (!file->stream.is_open())
        {
          ErrorService::runtime_error("File is not open", "");
        }

        if (!std::getline(file->stream, line))
        {
          return Value::nil_value(); // EOF
        }

        return Value::string_value(line);
      })
    });

    properties.get()->insert({
      "read_all", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = std::static_pointer_cast<FileHandler>(receiver.object.native_object);

        std::stringstream buffer;
        buffer << file->stream.rdbuf();

        return Value::string_value(buffer.str());
      })
    });

    properties.get()->insert({
      "close", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = std::static_pointer_cast<FileHandler>(receiver.object.native_object);

        file->stream.close();

        return Value::nil_value();
      })
    });

    properties.get()->insert({
      "has_next", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = std::static_pointer_cast<FileHandler>(receiver.object.native_object);

        return Value::boolean_value(file->stream.good());
      })
    });

    auto object = Value::object_value(properties);
    object.object.native_object = native_object;

    return object;
  }
};
