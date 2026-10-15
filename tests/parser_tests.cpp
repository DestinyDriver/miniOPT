#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "test_utils.h"

using namespace miniopt;

TEST_CASE(TestOperatorPrecedence) {
    std::string source = "int x = 10 + 20 * 3;";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();

    ASSERT_EQ(program->getStatements().size(), 1u);
    auto decl = dynamic_cast<const VariableDeclaration*>(program->getStatements()[0].get());
    ASSERT_TRUE(decl != nullptr);
    ASSERT_EQ(decl->getName(), "x");

    auto binPlus = dynamic_cast<const BinaryExpr*>(decl->getInitializer());
    ASSERT_TRUE(binPlus != nullptr);
    ASSERT_EQ(binPlus->getOp(), BinaryOp::ADD);

    auto leftNum = dynamic_cast<const NumberExpr*>(binPlus->getLeft());
    ASSERT_TRUE(leftNum != nullptr);
    ASSERT_EQ(leftNum->getValue(), 10);

    auto binMul = dynamic_cast<const BinaryExpr*>(binPlus->getRight());
    ASSERT_TRUE(binMul != nullptr);
    ASSERT_EQ(binMul->getOp(), BinaryOp::MUL);

    auto mulLeft = dynamic_cast<const NumberExpr*>(binMul->getLeft());
    auto mulRight = dynamic_cast<const NumberExpr*>(binMul->getRight());
    ASSERT_TRUE(mulLeft != nullptr && mulRight != nullptr);
    ASSERT_EQ(mulLeft->getValue(), 20);
    ASSERT_EQ(mulRight->getValue(), 3);
}

TEST_CASE(TestParenthesesPrecedence) {
    std::string source = "int x = (10 + 20) * 3;";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();

    auto decl = dynamic_cast<const VariableDeclaration*>(program->getStatements()[0].get());
    ASSERT_TRUE(decl != nullptr);

    auto binMul = dynamic_cast<const BinaryExpr*>(decl->getInitializer());
    ASSERT_TRUE(binMul != nullptr);
    ASSERT_EQ(binMul->getOp(), BinaryOp::MUL);

    auto binPlus = dynamic_cast<const BinaryExpr*>(binMul->getLeft());
    ASSERT_TRUE(binPlus != nullptr);
    ASSERT_EQ(binPlus->getOp(), BinaryOp::ADD);

    auto mulRight = dynamic_cast<const NumberExpr*>(binMul->getRight());
    ASSERT_TRUE(mulRight != nullptr);
    ASSERT_EQ(mulRight->getValue(), 3);
}

TEST_CASE(TestControlFlowAST) {
    std::string source = "int x = 5;\nif (x > 10) { x = x + 1; }\nwhile (x < 20) { x = x * 2; }";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();

    ASSERT_EQ(program->getStatements().size(), 3u);
    ASSERT_TRUE(dynamic_cast<const VariableDeclaration*>(program->getStatements()[0].get()) != nullptr);
    ASSERT_TRUE(dynamic_cast<const IfStatement*>(program->getStatements()[1].get()) != nullptr);
    ASSERT_TRUE(dynamic_cast<const WhileStatement*>(program->getStatements()[2].get()) != nullptr);
}

TEST_CASE(TestParserSyntaxErrorMissingSemicolon) {
    std::string source = "int x = 10";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    ASSERT_THROWS(parser.parse(), ParserError);
}

int main() {
    return printTestSummary("Parser Tests");
}
