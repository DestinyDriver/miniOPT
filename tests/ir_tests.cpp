#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ir/IRGenerator.h"
#include "test_utils.h"

using namespace miniopt;

TEST_CASE(TestArithmeticIR) {
    std::string source = "int a = 10 + 20 * 2;";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();

    IRGenerator irGen;
    auto tac = irGen.generate(*program);

    ASSERT_EQ(tac.size(), 3u);
    // t1 = 20 * 2
    ASSERT_EQ(tac.getInstructions()[0].op, OpCode::MUL);
    ASSERT_EQ(tac.getInstructions()[0].dest, "t1");
    ASSERT_EQ(tac.getInstructions()[0].src1, "20");
    ASSERT_EQ(tac.getInstructions()[0].src2, "2");

    // t2 = 10 + t1
    ASSERT_EQ(tac.getInstructions()[1].op, OpCode::ADD);
    ASSERT_EQ(tac.getInstructions()[1].dest, "t2");
    ASSERT_EQ(tac.getInstructions()[1].src1, "10");
    ASSERT_EQ(tac.getInstructions()[1].src2, "t1");

    // a = t2
    ASSERT_EQ(tac.getInstructions()[2].op, OpCode::MOV);
    ASSERT_EQ(tac.getInstructions()[2].dest, "a");
    ASSERT_EQ(tac.getInstructions()[2].src1, "t2");
}

TEST_CASE(TestUndefinedVariableThrows) {
    std::string source = "int a = b + 1;";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();

    IRGenerator irGen;
    ASSERT_THROWS(irGen.generate(*program), IRError);
}

TEST_CASE(TestControlFlowIR) {
    std::string source = "int x = 0;\nif (x > 10) { x = x + 1; }\nwhile (x < 20) { x = x + 1; }";
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();

    IRGenerator irGen;
    auto tac = irGen.generate(*program);

    bool hasJumpIfFalse = false;
    bool hasJump = false;
    bool hasLabel = false;

    for (const auto& instr : tac.getInstructions()) {
        if (instr.op == OpCode::JUMP_IF_FALSE) hasJumpIfFalse = true;
        if (instr.op == OpCode::JUMP) hasJump = true;
        if (instr.op == OpCode::LABEL) hasLabel = true;
    }

    ASSERT_TRUE(hasJumpIfFalse);
    ASSERT_TRUE(hasJump);
    ASSERT_TRUE(hasLabel);
}

int main() {
    return printTestSummary("IR Generator Tests");
}
