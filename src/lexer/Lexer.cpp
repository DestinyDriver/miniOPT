#include "lexer/Lexer.h"
#include <cctype>
#include <unordered_map>

namespace miniopt {

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"int", TokenType::INT},
    {"if", TokenType::IF},
    {"while", TokenType::WHILE}
};

Lexer::Lexer(std::string source)
    : source(std::move(source)), start(0), current(0), line(1), column(1), tokenStartColumn(1) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        tokens.push_back(scanToken());
    }
    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }
    return scanToken();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.size()) return '\0';
    return source[current + 1];
}

char Lexer::advance() {
    char c = source[current++];
    column++;
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    column++;
    return true;
}

bool Lexer::isAtEnd() const {
    return current >= source.size();
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            advance();
            line++;
            column = 1;
        } else if (c == '/') {
            if (peekNext() == '/') {
                // Line comment
                advance(); // consume '/'
                advance(); // consume '/'
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
            } else if (peekNext() == '*') {
                // Block comment
                advance(); // consume '/'
                advance(); // consume '*'
                while (!isAtEnd()) {
                    if (peek() == '*' && peekNext() == '/') {
                        advance(); // consume '*'
                        advance(); // consume '/'
                        break;
                    }
                    if (peek() == '\n') {
                        line++;
                        column = 0; // will be incremented by advance to 1
                    }
                    advance();
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

Token Lexer::scanToken() {
    start = current;
    tokenStartColumn = column;
    char c = advance();

    if (std::isalpha(c) || c == '_') {
        return identifier();
    }
    if (std::isdigit(c)) {
        return number();
    }

    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", line, tokenStartColumn);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", line, tokenStartColumn);
        case '{': return Token(TokenType::LEFT_BRACE, "{", line, tokenStartColumn);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", line, tokenStartColumn);
        case ';': return Token(TokenType::SEMICOLON, ";", line, tokenStartColumn);
        case '+': return Token(TokenType::PLUS, "+", line, tokenStartColumn);
        case '-': return Token(TokenType::MINUS, "-", line, tokenStartColumn);
        case '*': return Token(TokenType::MULTIPLY, "*", line, tokenStartColumn);
        case '/': return Token(TokenType::DIVIDE, "/", line, tokenStartColumn);
        case '%': return Token(TokenType::MODULO, "%", line, tokenStartColumn);

        case '=':
            if (match('=')) {
                return Token(TokenType::EQUAL_EQUAL, "==", line, tokenStartColumn);
            }
            return Token(TokenType::ASSIGN, "=", line, tokenStartColumn);

        case '!':
            if (match('=')) {
                return Token(TokenType::NOT_EQUAL, "!=", line, tokenStartColumn);
            }
            throw LexerError("Unexpected character '!'. Did you mean '!='?", line, tokenStartColumn);

        case '<':
            if (match('=')) {
                return Token(TokenType::LESS_EQUAL, "<=", line, tokenStartColumn);
            }
            return Token(TokenType::LESS, "<", line, tokenStartColumn);

        case '>':
            if (match('=')) {
                return Token(TokenType::GREATER_EQUAL, ">=", line, tokenStartColumn);
            }
            return Token(TokenType::GREATER, ">", line, tokenStartColumn);

        default:
            throw LexerError(std::string("Unexpected character '") + c + "'", line, tokenStartColumn);
    }
}

Token Lexer::number() {
    while (std::isdigit(peek())) {
        advance();
    }
    std::string text = source.substr(start, current - start);
    return Token(TokenType::NUMBER, text, line, tokenStartColumn);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string text = source.substr(start, current - start);
    auto it = KEYWORDS.find(text);
    if (it != KEYWORDS.end()) {
        return Token(it->second, text, line, tokenStartColumn);
    }
    return Token(TokenType::IDENTIFIER, text, line, tokenStartColumn);
}

} // namespace miniopt
