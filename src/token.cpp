#include "./token.hpp"

std::string token_type_to_string(TokenType type)
{
  switch (type)
  {
    case TokenType::Func:
      return "Func";
    case TokenType::Let:
      return "Let";
    case TokenType::If:
      return "If";
    case TokenType::Else:
      return "Else";
    case TokenType::ElseIf:
      return "ElseIf";
    case TokenType::While:
      return "While";
    case TokenType::Identifier:
      return "Identifier";
    case TokenType::Number:
      return "Number";
    case TokenType::String:
      return "String";
    case TokenType::Plus:
      return "Plus";
    case TokenType::Minus:
      return "Minus";
    case TokenType::Star:
      return "Star";
    case TokenType::Slash:
      return "Slash";
    case TokenType::Equal:
      return "Equal";
    case TokenType::LeftParen:
      return "LeftParen";
    case TokenType::RightParen:
      return "RightParen";
    case TokenType::LeftBrace:
      return "LeftBrace";
    case TokenType::RightBrace:
      return "RightBrace";
    case TokenType::Comma:
      return "Comma";
    case TokenType::Semicolon:
      return "Semicolon";
    case TokenType::DoubleQuote:
      return "DoubleQuote";
    case TokenType::Concat:
      return "Concat";
    case TokenType::Greater:
      return "Greater";
    case TokenType::GreaterEqual:
      return "GreaterEqual";
    case TokenType::Less:
      return "Less";
    case TokenType::LessEqual:
      return "LessEqual";
    case TokenType::EqualEqual:
      return "EqualEqual";
    case TokenType::Bang:
      return "Bang";
    case TokenType::BangEqual:
      return "BangEqual";
    case TokenType::Return:
      return "Return";
    case TokenType::Comment:
      return "Comment";
    case TokenType::Dot:
      return "Dot";
    case TokenType::ColonColon:
      return "ColonColon";
    case TokenType::EndOfFile:
      return "EndOfFile";
    case TokenType::Unknown:
      return "Unknown";
    default:
      return "Invalid TokenType";
  }
}
