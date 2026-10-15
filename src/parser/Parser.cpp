#include "parser/Parser.h"
#include <stdexcept>

namespace miniopt {

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)), current(0) {}

std::unique_ptr<Program> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<Program>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (check(TokenType::INT)) {
        return parseVariableDeclaration();
    }
    if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }
    if (check(TokenType::IF)) {
        return parseIfStatement();
    }
    if (check(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    if (check(TokenType::LEFT_BRACE)) {
        return parseBlockStatement();
    }

    const Token& tok = peek();
    throw ParserError("Unexpected token '" + (tok.lexeme.empty() ? tokenTypeToString(tok.type) : tok.lexeme) + "'",
                      tok.line, tok.column);
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    consume(TokenType::INT, "Expected 'int' keyword");
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name after 'int'");

    std::unique_ptr<Expression> init = nullptr;
    if (match(TokenType::ASSIGN)) {
        init = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

    return std::make_unique<VariableDeclaration>("int", nameToken.lexeme, std::move(init));
}

std::unique_ptr<Statement> Parser::parseAssignment() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name");
    consume(TokenType::ASSIGN, "Expected '=' in assignment");
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");

    return std::make_unique<Assignment>(nameToken.lexeme, std::move(value));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'if'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition");
    auto thenBranch = parseBlockStatement();

    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch));
}

std::unique_ptr<Statement> Parser::parseWhileStatement() {
    consume(TokenType::WHILE, "Expected 'while'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition");
    auto body = parseBlockStatement();

    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    consume(TokenType::LEFT_BRACE, "Expected '{' to start block");
    std::vector<std::unique_ptr<Statement>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to close block");
    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expr = parseAddition();

    while (check(TokenType::LESS) || check(TokenType::GREATER) ||
           check(TokenType::LESS_EQUAL) || check(TokenType::GREATER_EQUAL) ||
           check(TokenType::EQUAL_EQUAL) || check(TokenType::NOT_EQUAL)) {
        Token opToken = advance();
        BinaryOp op;
        switch (opToken.type) {
            case TokenType::LESS:          op = BinaryOp::LESS; break;
            case TokenType::GREATER:       op = BinaryOp::GREATER; break;
            case TokenType::LESS_EQUAL:    op = BinaryOp::LESS_EQUAL; break;
            case TokenType::GREATER_EQUAL: op = BinaryOp::GREATER_EQUAL; break;
            case TokenType::EQUAL_EQUAL:   op = BinaryOp::EQUAL_EQUAL; break;
            case TokenType::NOT_EQUAL:     op = BinaryOp::NOT_EQUAL; break;
            default:                       op = BinaryOp::ADD; break;
        }
        auto right = parseAddition();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseAddition() {
    auto expr = parseMultiplication();

    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        Token opToken = advance();
        BinaryOp op = (opToken.type == TokenType::PLUS) ? BinaryOp::ADD : BinaryOp::SUB;
        auto right = parseMultiplication();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseMultiplication() {
    auto expr = parseUnary();

    while (check(TokenType::MULTIPLY) || check(TokenType::DIVIDE) || check(TokenType::MODULO)) {
        Token opToken = advance();
        BinaryOp op;
        if (opToken.type == TokenType::MULTIPLY) {
            op = BinaryOp::MUL;
        } else if (opToken.type == TokenType::DIVIDE) {
            op = BinaryOp::DIV;
        } else {
            op = BinaryOp::MOD;
        }
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::MINUS)) {
        auto right = parseUnary();
        return std::make_unique<BinaryExpr>(BinaryOp::SUB, std::make_unique<NumberExpr>(0), std::move(right));
    }
    if (match(TokenType::PLUS)) {
        return parseUnary();
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        int val = 0;
        try {
            val = std::stoi(previous().lexeme);
        } catch (...) {
            throw ParserError("Integer literal out of range: " + previous().lexeme, previous().line, previous().column);
        }
        return std::make_unique<NumberExpr>(val);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(previous().lexeme);
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }

    const Token& tok = peek();
    throw ParserError("Expected expression but found '" + (tok.lexeme.empty() ? tokenTypeToString(tok.type) : tok.lexeme) + "'",
                      tok.line, tok.column);
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        return advance();
    }
    const Token& tok = peek();
    throw ParserError(errorMessage + " (found '" + (tok.lexeme.empty() ? tokenTypeToString(tok.type) : tok.lexeme) + "')",
                      tok.line, tok.column);
}

} // namespace miniopt
