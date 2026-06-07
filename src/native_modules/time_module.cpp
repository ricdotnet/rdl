#include <chrono>
#include <thread>
#include <vector>
#include "./native_module.hpp"

class TimeModule : NativeModule
{
public:
  explicit TimeModule() : NativeModule() {}

  [[nodiscard]] std::string name() const override { return "time"; }

  [[nodiscard]] Value init() override
  {
    const auto functions = std::make_shared<std::unordered_map<std::string, Value> >();

    (*functions)["now"] = Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) {
      return now(args);
    });
    (*functions)["sleep"] = Value::builtin_function_value([](const Value &receiver, const std::vector<Value> &args) {
      return sleep(args);
    });

    return Value::object_value(functions);
  }

private:
  static Value now(const std::vector<Value> &args)
  {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for now, found ", std::to_string(args.size()));
    }
    const long current_time = std::chrono::system_clock::now().time_since_epoch().count();
    return Value::number_value(current_time);
  }

  static Value sleep(const std::vector<Value> &args)
  {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for sleep, found ", std::to_string(args.size()));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(args[0].number));
    return Value::nil_value();
  }
};
