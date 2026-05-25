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

  // std::cout << "Tokens:" << std::endl;
  // for (const auto &token: tokens) {
  //   std::cout << token_type_to_string(token.type) << ": " << token.value << std::endl;
  // }

  const auto program = parser.parse();

  Environment env;
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
