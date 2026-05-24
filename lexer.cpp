#include "lexer.hpp"
#include "token.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

bool Lexer::is_digit(char c) { return std::isdigit(c); }
bool Lexer::is_alpha(char c) { return std::isalpha(c); }

Lexer::Lexer(std::string src) : source(std::move(src)) {
};

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (current < source.size()) {
        char c = source[current];

        if (std::isspace(c)) {
            current++;
            continue;
        }

        // this allows an identifier to start with an alpha char only while allowing
        // numbers anywhere else will not allow any invalid character though
        if (is_alpha(c) || c == '_') {
            std::string value;

            while (current < source.size() && (is_alpha(source[current]) || is_digit(source[current]) || source[
                                                   current] == '_')) {
                value += source[current];
                current++;
            }

            tokens.push_back({TokenType::Identifier, value});

            continue;
        }

        if (is_digit(c)) {
            std::string number;

            while (current < source.size() && is_digit(source[current])) {
                number += source[current];
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

        switch (c) {
            // case '"':
            //     tokens.push_back({TokenType::DoubleQuote, "\""});
            //     break;
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
