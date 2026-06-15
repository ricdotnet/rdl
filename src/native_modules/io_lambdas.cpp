#include "./io_lambdas.hpp"
#include "../interpreter.hpp"

NativeFn print(RuntimeContext context)
{
  return [context](const Value &, const std::vector<Value> &args) {
    for (const auto &arg: args)
    {
      *context.runtime->out << arg.to_string();
    }

    return Value::nil_value();
  };
}

NativeFn println(RuntimeContext context)
{
  return [context](const Value &, const std::vector<Value> &args) {
    for (const auto &arg: args)
    {
      *context.runtime->out << arg.to_string();
    }
    *context.runtime->out << std::endl;

    return Value::nil_value();
  };
}

NativeFn input()
{
  return [](const Value &, const std::vector<Value> &) {
    std::string input;
    std::getline(std::cin, input);
    return Value::string_value(input);
  };
}
