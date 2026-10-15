#ifndef MINIO_TOKEN_H
#define MINIO_TOKEN_H

#include <string>
#include <iostream>

namespace miniopt {

enum class TokenType {
    // Keywords & Types
    INT,
    IF,
    WHILE,

    // Literals & Identifiers
    IDENTIFIER,
    NUMBER,

    // Arithmetic Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %

    // Assignment Operator
    ASSIGN,         // =

    // Comparison Operators
    LESS,           // <
    GREATER,        // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=

    // Delimiters & Punctuation
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    SEMICOLON,      // ;

    // Special
    END_OF_FILE,
    UNKNOWN
};

std::string tokenTypeToString(TokenType type);

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type = TokenType::UNKNOWN, std::string lexeme = "", int line = 1, int column = 1)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}

    std::string toString() const;
};

} // namespace miniopt

#endif // MINIO_TOKEN_H
