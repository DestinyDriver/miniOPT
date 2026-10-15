#ifndef MINIO_LEXER_H
#define MINIO_LEXER_H

#include "lexer/Token.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace miniopt {

class LexerError : public std::runtime_error {
public:
    LexerError(const std::string& message, int line, int column)
        : std::runtime_error("Lexer Error at line " + std::to_string(line) +
                             ", column " + std::to_string(column) + ": " + message),
          line(line), column(column) {}

    int getLine() const { return line; }
    int getColumn() const { return column; }

private:
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();
    Token nextToken();

private:
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token scanToken();
    Token number();
    Token identifier();

    std::string source;
    size_t start;
    size_t current;
    int line;
    int column;
    int tokenStartColumn;
};

} // namespace miniopt

#endif // MINIO_LEXER_H
