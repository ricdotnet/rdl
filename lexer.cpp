#include "lexer.hpp"
#include "token.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

#include "error_service.hpp"

bool Lexer::is_digit(char c) { return std::isdigit(c); }
bool Lexer::is_alpha(char c) { return std::isalpha(c); }
char Lexer::peek(const std::string &source, const size_t index) { return source[index]; }

Lexer::Lexer(std::string src) : source(std::move(src)) {
};

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (current < source.size()) {
        const char c = peek(source, current);

        if (std::isspace(c)) {
            current++;
            continue;
        }

        // this allows an identifier to start with an alpha char only while allowing
        // numbers anywhere else will not allow any invalid character though
        if (is_alpha(c) || c == '_') {
            std::string value;

            while (current < source.size() && (is_alpha(peek(source, current)) || is_digit(peek(source, current)) ||
                                               peek(source, current) == '_')) {
                value += peek(source, current);
                current++;
            }

            if (value == "let") {
                tokens.push_back({TokenType::Let, value, 1, current});
                continue;
            }

            tokens.push_back({TokenType::Identifier, value, 1, current});

            continue;
        }

        if (is_digit(c)) {
            std::string number;

            while (current < source.size() && is_digit(peek(source, current))) {
                number += peek(source, current);
                current++;
            }

            tokens.push_back({TokenType::Number, number, 1, current});

            continue;
        }

        if (c == '"') {
            std::string value;
            current++;

            while (current < source.size() && source[current] != '"') {
                value += source[current];
                current++;
            }

            if (current >= source.size()) {
                ErrorService::syntax_error("Unterminated string", {TokenType::String, value, 1, current});
            }

            current++;
            tokens.push_back({TokenType::String, value, 1, current});

            continue;
        }

        if (c == '.' && peek(source, current + 1) == '.') {
            current += 2;
            tokens.push_back({TokenType::Concat, "..", 1, current});
            continue;
        }

        switch (c) {
            case '+':
                tokens.push_back({TokenType::Plus, "+", 1, current});
                break;
            case '-':
                tokens.push_back({TokenType::Minus, "-", 1, current});
                break;
            case '*':
                tokens.push_back({TokenType::Star, "*", 1, current});
                break;
            case '/':
                tokens.push_back({TokenType::Slash, "/", 1, current});
                break;
            case '(':
                tokens.push_back({TokenType::LeftParen, "(", 1, current});
                break;
            case ')':
                tokens.push_back({TokenType::RightParen, ")", 1, current});
                break;
            case '=':
                tokens.push_back({TokenType::Equals, "=", 1, current});
                break;
            case ',':
                tokens.push_back({TokenType::Comma, ",", 1, current});
                break;
            case ';':
                tokens.push_back({TokenType::Semicolon, ";", 1, current});
                break;
            default:
                ErrorService::syntax_error("Unexpected character: " + std::string(1, c),
                                           {TokenType::Unknown, std::string(1, c), 1, current});
        }

        current++;
    }

    tokens.push_back({TokenType::EndOfFile, "", 1, current});

    return tokens;
}
