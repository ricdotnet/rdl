#include <utility>

#include "ast_printer.cpp"
#include "environment.hpp"
#include "interpreter.cpp"
#include "lexer.hpp"
#include "parser.hpp"

void run(std::string source) {
    Lexer lexer(std::move(source));
    Parser parser(lexer.tokenize());

    const auto program = parser.parse();

    Environment env;
    Interpreter interpreter(env);

    for (auto &statement: program) {
        const auto stmt = statement.get();
        interpreter.evaluate(stmt);
    }
}

int main() {
    // std::string source = R"(name = "Ricardo"; surname = "Rocha"; print("Name: " .. name .. " " .. surname .. " :: " .. "Age: " .. 25))";
    // std::string source = R"(print(1 + 2 * 3))";
    std::string source = "print(10 .. 20 + 30)";

    // auto env = Environment();
    //
    // Lexer lexer(source);
    // const auto tokens = lexer.tokenize();
    //
    // // TODO: add debug flag
    // // std::cout << "Tokens: \n" << std::endl;
    // // for (const auto &token: tokens) {
    // //     std::cout << token_type_to_string(token.type) << " : " << token.value << '\n';
    // // }
    // // std::cout << std::endl;
    //
    // Parser parser(tokens);
    // const auto program = parser.parse();
    //
    // ASTPrinter printer;
    // Interpreter interpreter(env);
    //
    // for (auto &statement: program) {
    //     const auto stmt = statement.get();
    //     // stmt->accept(printer);
    //     interpreter.evaluate(stmt);
    // }
    
    run(source);

    return 0;
}
