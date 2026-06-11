#include "./time_lambdas.hpp"
#include <chrono>
#include <thread>
#include <vector>

NativeFn now()
{
  return [](const Value &, const std::vector<Value> &args) {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for now, found ", std::to_string(args.size()));
    }
    const long current_time = std::chrono::system_clock::now().time_since_epoch().count();
    return Value::number_value(current_time);
  };
}

NativeFn sleep()
{
  return [](const Value &, const std::vector<Value> &args) {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for sleep, found ", std::to_string(args.size()));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(args[0].number));
    return Value::nil_value();
  };
}
