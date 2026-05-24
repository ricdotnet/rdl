#include "ast_printer.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>

int main() {
    std::string source = "x = 10; z = x + 10; a = x + z; h = \"hello world\"; print(h);";

    auto env = Environment();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // TODO: add debug flag
    std::cout << "Tokens: \n" << std::endl;
    for (const auto &token: tokens) {
        std::cout << token_type_to_string(token.type) << " : " << token.value << '\n';
    }
    std::cout << std::endl;

    Parser parser(tokens);
    auto program = parser.parse();

    for (auto &statement: program) {
        auto stmt = statement.get();

        // TODO: add debug flag
        print_ast(stmt, 0);

        evaluate(stmt, env);
    }

    return 0;
}
