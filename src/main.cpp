#include <utility>
#include "./environment.hpp"
#include "./interpreter.cpp"
#include "./io.hpp"
#include "./lexer.hpp"
#include "./parser.hpp"

void run(std::string source) {
  Lexer lexer(std::move(source));
  auto tokens = lexer.tokenize();
  Parser parser(tokens);

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
