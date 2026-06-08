#include "./native_module.hpp"

class IoModule : public NativeModule
{
public:
  explicit IoModule() : NativeModule() {}

  ~IoModule() override = default;

  [[nodiscard]] std::string name() const override
  {
    return "io";
  }

  [[nodiscard]] Value init() override
  {
    const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

    (*functions)["print"] = Value::builtin_function_value([](const Value &, const std::vector<Value> &args) {
      return print(args);
    });

    (*functions)["println"] = Value::builtin_function_value([](const Value &, const std::vector<Value> &args) {
      return println(args);
    });

    (*functions)["input"] = Value::builtin_function_value([](const Value &, const std::vector<Value> &) {
      return input();
    });

    return Value::object_value(functions);
  }

private:
  static Value print(const std::vector<Value> &args)
  {
    for (const auto &arg: args)
    {
      std::cout << arg.to_string();
    }
    return Value::nil_value();
  }

  static Value println(const std::vector<Value> &args)
  {
    for (const auto &arg: args)
    {
      std::cout << arg.to_string();
    }
    std::cout << std::endl;
    return Value::nil_value();
  }

  static Value input()
  {
    std::string input;
    std::getline(std::cin, input);
    return Value::string_value(input);
  }
};
