#ifndef MINIO_PARSER_H
#define MINIO_PARSER_H

#include "lexer/Token.h"
#include "parser/AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace miniopt {

class ParserError : public std::runtime_error {
public:
    ParserError(const std::string& message, int line, int column)
        : std::runtime_error("Parser Error at line " + std::to_string(line) +
                             ", column " + std::to_string(column) + ": " + message),
          line(line), column(column) {}

    int getLine() const { return line; }
    int getColumn() const { return column; }

private:
    int line;
    int column;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<Program> parse();

private:
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseVariableDeclaration();
    std::unique_ptr<Statement> parseAssignment();
    std::unique_ptr<Statement> parseIfStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<BlockStatement> parseBlockStatement();

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseAddition();
    std::unique_ptr<Expression> parseMultiplication();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();

    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& errorMessage);

    std::vector<Token> tokens;
    size_t current;
};

} // namespace miniopt

#endif // MINIO_PARSER_H
