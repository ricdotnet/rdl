#include <utility>
#include "src/environment.hpp"
#include "src/interpreter.cpp"
#include "src/io.hpp"
#include "src/lexer.hpp"
#include "src/parser.hpp"

void run(std::string source) {
  Lexer lexer(std::move(source));
  auto tokens = lexer.tokenize();
  Parser parser(tokens);

  std::cout << "Tokens: " << std::endl;
  std::cout << std::endl;
  for (const auto &token: tokens) {
    std::cout << token_type_to_string(token.type) + ": " << token.value << std::endl;
  }
  std::cout << std::endl;

  const auto program = parser.parse();

  Environment env;

  env.define_builtin("print", [](const std::vector<Value> &args) -> Value {
    for (const auto &arg: args) {
      std::cout << arg.to_string() << " ";
    }
    std::cout << std::endl;
    return Value::nil_value();
  });

  Interpreter interpreter(env);

  for (auto &statement: program) {
    const auto stmt = statement.get();
    interpreter.evaluate(stmt);
  }
}

int main(const int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: ./language <file>\n";
    return 1;
  }

  const std::string source = read_file(argv[1]);

  run(source);

  return 0;
}
