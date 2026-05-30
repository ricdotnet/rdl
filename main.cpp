#include <chrono>
#include <thread>
#include <utility>
#include "src/environment.hpp"
#include "src/interpreter.cpp"
#include "src/io.hpp"
#include "src/lexer.hpp"
#include "src/parser.hpp"
#include "src/runtime.hpp"

void run(std::string source, const bool debug)
{
  Lexer lexer(std::move(source));
  auto tokens = lexer.tokenize();
  Parser parser(tokens);

  if (debug)
  {
    std::cout << "Tokens: " << std::endl;
    std::cout << std::endl;
    for (const auto &token: tokens)
    {
      std::cout << token_type_to_string(token.type) + ": " << token.value << std::endl;
    }
    std::cout << std::endl;
  }

  const auto program = parser.parse();

  Environment env;
  Runtime runtime;

  runtime.define_builtin("print", [](const std::vector<Value> &args) -> Value {
    for (const auto &arg: args)
    {
      std::cout << arg.to_string() << " ";
    }
    std::cout << std::endl;
    return Value::nil_value();
  });

  runtime.define_builtin("sleep", [](const std::vector<Value> &args) -> Value {
    if (args.size() != 1)
    {
      ErrorService::runtime_error("Expected 1 argument for sleep in milliseconds.",
                                  "Found " + std::to_string(args.size()));
    }

    const auto duration_value = &args[0];
    if (!duration_value->is_number())
    {
      ErrorService::runtime_error("Expected number in milliseconds for sleep duration.",
                                  "Found " + Value::type_name(duration_value->type));
    }

    const auto duration = duration_value->number;
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));

    return Value::nil_value();
  });

  runtime.define_builtin("now", [](const std::vector<Value> &args) -> Value {
    if (!args.empty())
    {
      ErrorService::runtime_error("Expected 0 arguments for now, found ", std::to_string(args.size()));
    }

    return Value::number_value(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).
      count());
  });

  Interpreter interpreter(&env, &runtime);

  for (auto &statement: program)
  {
    const auto stmt = statement.get();
    interpreter.evaluate(stmt);
  }
}

int main(const int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: ./language <file>\n";
    return 1;
  }

  const std::string source = read_file(argv[1]);

  const bool debug = argc > 2 && std::string(argv[2]) == "--debug";

  run(source, debug);

  return 0;
}
