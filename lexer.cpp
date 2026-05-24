#include "lexer.hpp"
#include "token.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

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

            tokens.push_back({TokenType::Identifier, value});

            continue;
        }

        if (is_digit(c)) {
            std::string number;

            while (current < source.size() && is_digit(peek(source, current))) {
                number += peek(source, current);
                current++;
            }

            tokens.push_back({TokenType::Number, number});

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
                throw std::runtime_error("Unterminated string");
            }

            current++;
            tokens.push_back({TokenType::String, value});

            continue;
        }

        if (c == '.' && peek(source, current + 1) == '.') {
            current += 2;
            tokens.push_back({TokenType::Concat, ".."});
            continue;
        }

        switch (c) {
            case '+':
                tokens.push_back({TokenType::Plus, "+"});
                break;
            case '-':
                tokens.push_back({TokenType::Minus, "-"});
                break;
            case '*':
                tokens.push_back({TokenType::Star, "*"});
                break;
            case '/':
                tokens.push_back({TokenType::Slash, "/"});
                break;
            case '(':
                tokens.push_back({TokenType::LeftParen, "("});
                break;
            case ')':
                tokens.push_back({TokenType::RightParen, ")"});
                break;
            case '=':
                tokens.push_back({TokenType::Equals, "="});
                break;
            case ',':
                tokens.push_back({TokenType::Comma, ","});
                break;
            case ';':
                tokens.push_back({TokenType::Semicolon, ";"});
                break;
            default:
                throw std::runtime_error("Unknown character: " + std::string(1, c));
        }

        current++;
    }

    tokens.push_back({TokenType::EndOfFile, ""});

    return tokens;
}
