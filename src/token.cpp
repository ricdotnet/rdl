#include "./token.hpp"

std::string token_type_to_string(TokenType type)
{
  switch (type)
  {
    case TokenType::Unknown:
      return "Unknown";
    case TokenType::EndOfFile:
      return "EndOfFile";
    case TokenType::Comment:
      return "Comment";

    // IDENTIFIER LITERALS
    case TokenType::Identifier:
      return "Identifier";
    case TokenType::Number:
      return "Number";
    case TokenType::String:
      return "String";
    case TokenType::True:
      return "True";
    case TokenType::False:
      return "False";
    case TokenType::HttpVerb:
      return "HttpVerb";

    // OPERATORS
    case TokenType::And:
      return "And";
    case TokenType::Or:
      return "Or";
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

    // DELIMITERS
    case TokenType::LeftParen:
      return "LeftParen";
    case TokenType::RightParen:
      return "RightParen";
    case TokenType::LeftBrace:
      return "LeftBrace";
    case TokenType::RightBrace:
      return "RightBrace";
    case TokenType::LeftBracket:
      return "LeftBracket";
    case TokenType::RightBracket:
      return "RightBracket";
    case TokenType::Comma:
      return "Comma";
    case TokenType::Semicolon:
      return "Semicolon";
    case TokenType::SingleQuote:
      return "SingleQuote";
    case TokenType::DoubleQuote:
      return "DoubleQuote";
    case TokenType::Backtick:
      return "Backtick";
    case TokenType::Concat:
      return "Concat";
    case TokenType::Dot:
      return "Dot";
    case TokenType::Colon:
      return "Colon";
    case TokenType::ColonColon:
      return "ColonColon";

    // KEYWORDS
    case TokenType::Import:
      return "Import";
    case TokenType::Func:
      return "Func";
    case TokenType::Let:
      return "Let";
    case TokenType::Struct:
      return "Struct";
    case TokenType::If:
      return "If";
    case TokenType::Else:
      return "Else";
    case TokenType::ElseIf:
      return "ElseIf";
    case TokenType::For:
      return "For";
    case TokenType::In:
      return "In";
    case TokenType::Range:
      return "Range";
    case TokenType::While:
      return "While";
    case TokenType::Return:
      return "Return";
    case TokenType::Group:
      return "Group";
    case TokenType::Route:
      return "Route";

    default:
      return "Invalid TokenType";
  }
}
