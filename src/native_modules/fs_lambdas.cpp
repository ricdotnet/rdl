#include "./fs_lambdas.hpp"
#include <sstream>
#include "../interpreter.hpp"
#include "../utils.hpp"

NativeFn read_file()
{
  return [](const Value &, std::vector<Value> &args) {
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

    file.close();

    return Value::string_value(buffer.str());
  };
}

NativeFn write_file()
{
  return [](const Value &, const std::vector<Value> &) {
    ErrorService::runtime_error("Not implemented", "");
    return Value::nil_value();
  };
}

NativeFn open_file()
{
  return [](const Value &, const std::vector<Value> &args) {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for read_file, found ", std::to_string(args.size()));
    }

    const std::string path = args[0].string;

    const auto native_object = std::make_shared<FileHandler>();
    native_object->stream.open(path);

    const auto properties = std::make_shared<std::unordered_map<std::string, Value> >();

    properties->insert({
      "read_line", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = native<FileHandler>(receiver);

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

    properties->insert({
      "read_all", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = native<FileHandler>(receiver);

        std::stringstream buffer;
        buffer << file->stream.rdbuf();

        return Value::string_value(buffer.str());
      })
    });

    properties->insert({
      "close", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = native<FileHandler>(receiver);

        file->stream.close();

        return Value::nil_value();
      })
    });

    properties->insert({
      "has_next", Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &) {
        const auto file = native<FileHandler>(receiver);

        return Value::boolean_value(file->stream.good());
      })
    });

    auto object = Value::object_value(properties);
    object.object.native_object = native_object;

    return object;
  };
}
