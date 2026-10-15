#include "lexer/Lexer.h"
#include "test_utils.h"

using namespace miniopt;

TEST_CASE(TestBasicTokenization) {
    std::string source = "int x = 10;\nx = x + 5;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 12u);
    ASSERT_EQ(tokens[0].type, TokenType::INT);
    ASSERT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[1].lexeme, "x");
    ASSERT_EQ(tokens[2].type, TokenType::ASSIGN);
    ASSERT_EQ(tokens[3].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[3].lexeme, "10");
    ASSERT_EQ(tokens[4].type, TokenType::SEMICOLON);

    ASSERT_EQ(tokens[5].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[5].lexeme, "x");
    ASSERT_EQ(tokens[6].type, TokenType::ASSIGN);
    ASSERT_EQ(tokens[7].type, TokenType::IDENTIFIER);
    ASSERT_EQ(tokens[7].lexeme, "x");
    ASSERT_EQ(tokens[8].type, TokenType::PLUS);
    ASSERT_EQ(tokens[9].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[9].lexeme, "5");
    ASSERT_EQ(tokens[10].type, TokenType::SEMICOLON);
    ASSERT_EQ(tokens[11].type, TokenType::END_OF_FILE);
}

TEST_CASE(TestArithmeticAndComments) {
    std::string source = "// Line comment\n10 + 20 * 30 / 4 % 2; /* block comment */";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens[0].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[0].lexeme, "10");
    ASSERT_EQ(tokens[1].type, TokenType::PLUS);
    ASSERT_EQ(tokens[2].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[2].lexeme, "20");
    ASSERT_EQ(tokens[3].type, TokenType::MULTIPLY);
    ASSERT_EQ(tokens[4].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[4].lexeme, "30");
    ASSERT_EQ(tokens[5].type, TokenType::DIVIDE);
    ASSERT_EQ(tokens[6].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[6].lexeme, "4");
    ASSERT_EQ(tokens[7].type, TokenType::MODULO);
    ASSERT_EQ(tokens[8].type, TokenType::NUMBER);
    ASSERT_EQ(tokens[8].lexeme, "2");
    ASSERT_EQ(tokens[9].type, TokenType::SEMICOLON);
}

TEST_CASE(TestComparisons) {
    std::string source = "< > <= >= == !=";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens[0].type, TokenType::LESS);
    ASSERT_EQ(tokens[1].type, TokenType::GREATER);
    ASSERT_EQ(tokens[2].type, TokenType::LESS_EQUAL);
    ASSERT_EQ(tokens[3].type, TokenType::GREATER_EQUAL);
    ASSERT_EQ(tokens[4].type, TokenType::EQUAL_EQUAL);
    ASSERT_EQ(tokens[5].type, TokenType::NOT_EQUAL);
}

TEST_CASE(TestKeywordsAndPunctuation) {
    std::string source = "if (x == 1) { while (y > 0) { y = y - 1; } }";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens[0].type, TokenType::IF);
    ASSERT_EQ(tokens[1].type, TokenType::LEFT_PAREN);
    ASSERT_EQ(tokens[5].type, TokenType::RIGHT_PAREN);
    ASSERT_EQ(tokens[6].type, TokenType::LEFT_BRACE);
    ASSERT_EQ(tokens[7].type, TokenType::WHILE);
}

TEST_CASE(TestInvalidCharacterThrows) {
    std::string source = "int x = @10;";
    Lexer lexer(source);
    ASSERT_THROWS(lexer.tokenize(), LexerError);
}

int main() {
    return printTestSummary("Lexer Tests");
}
