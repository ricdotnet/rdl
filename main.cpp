#include "ast_printer.cpp"
#include "environment.hpp"
#include "interpreter.cpp"
#include "lexer.hpp"
#include "parser.hpp"

int main() {
    std::string source = "x = 10; z = x + 10; a = x + z; h = \"hello worlds\"; print(h);";

    auto env = Environment();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // TODO: add debug flag
    // std::cout << "Tokens: \n" << std::endl;
    // for (const auto &token: tokens) {
    //     std::cout << token_type_to_string(token.type) << " : " << token.value << '\n';
    // }
    // std::cout << std::endl;

    Parser parser(tokens);
    auto program = parser.parse();

    ASTPrinter printer;
    Interpreter interpreter(env);

    for (auto &statement: program) {
        auto stmt = statement.get();
        // stmt->accept(printer);
        interpreter.evaluate(stmt);
    }

    return 0;
}
