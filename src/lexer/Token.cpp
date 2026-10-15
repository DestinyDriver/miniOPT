#include "lexer/Token.h"
#include <sstream>

namespace miniopt {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::INT:           return "INT";
        case TokenType::IF:            return "IF";
        case TokenType::WHILE:         return "WHILE";
        case TokenType::IDENTIFIER:    return "IDENTIFIER";
        case TokenType::NUMBER:        return "NUMBER";
        case TokenType::PLUS:          return "PLUS";
        case TokenType::MINUS:         return "MINUS";
        case TokenType::MULTIPLY:      return "MULTIPLY";
        case TokenType::DIVIDE:        return "DIVIDE";
        case TokenType::MODULO:        return "MODULO";
        case TokenType::ASSIGN:        return "ASSIGN";
        case TokenType::LESS:          return "LESS";
        case TokenType::GREATER:       return "GREATER";
        case TokenType::LESS_EQUAL:    return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::EQUAL_EQUAL:   return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL:     return "NOT_EQUAL";
        case TokenType::LEFT_PAREN:    return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:   return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:    return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:   return "RIGHT_BRACE";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::END_OF_FILE:   return "END_OF_FILE";
        case TokenType::UNKNOWN:       return "UNKNOWN";
        default:                       return "UNDEFINED";
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << tokenTypeToString(type) << ", \"" << lexeme << "\", line " << line << ", col " << column << ")";
    return oss.str();
}

} // namespace miniopt
