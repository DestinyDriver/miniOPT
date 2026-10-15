#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "ir/IRGenerator.h"
#include "optimizer/Optimizer.h"
#include "test_utils.h"

using namespace miniopt;

static TACProgram compileToTAC(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto program = parser.parse();
    IRGenerator irGen;
    return irGen.generate(*program);
}

TEST_CASE(TestConstantFolding) {
    // 10 + 20 * 3 should fold to 70
    std::string source = "int x = 10 + 20 * 3;";
    auto tac = compileToTAC(source);

    Optimizer optimizer;
    auto optTac = optimizer.optimize(tac);

    // After folding and propagation, x should be assigned 70
    bool found70 = false;
    for (const auto& instr : optTac.getInstructions()) {
        if (instr.dest == "x" && instr.src1 == "70") {
            found70 = true;
        }
    }
    ASSERT_TRUE(found70);
}

TEST_CASE(TestAlgebraicSimplification) {
    std::string source = "int x = 5;\nint a = x + 0;\nint b = x * 1;\nint c = x * 0;\nint res = a + b + c;";
    auto tac = compileToTAC(source);

    Optimizer optimizer;
    auto optTac = optimizer.optimize(tac);

    // After full optimization, known constant 5 is propagated and folded into res
    // Specifically verify algebraic simplification removed multiplications by 1 and additions of 0
    for (const auto& instr : optTac.getInstructions()) {
        if (instr.op == OpCode::ADD) {
            ASSERT_TRUE(instr.src1 != "0" && instr.src2 != "0");
        }
        if (instr.op == OpCode::MUL) {
            ASSERT_TRUE(instr.src1 != "1" && instr.src2 != "1");
            ASSERT_TRUE(instr.src1 != "0" && instr.src2 != "0");
        }
    }
}

TEST_CASE(TestConstantPropagation) {
    std::string source = "int a = 10;\nint b = a + 5;\nint c = b * 2;";
    auto tac = compileToTAC(source);

    OptimizerOptions opts;
    opts.enableDeadCodeElimination = false;
    Optimizer optimizer(opts);
    auto optTac = optimizer.optimize(tac);

    // a = 10, b = 15, c = 30
    bool foundA = false, foundB = false, foundC = false;
    for (const auto& instr : optTac.getInstructions()) {
        if (instr.dest == "a" && instr.src1 == "10") foundA = true;
        if (instr.dest == "b" && instr.src1 == "15") foundB = true;
        if (instr.dest == "c" && instr.src1 == "30") foundC = true;
    }
    ASSERT_TRUE(foundA);
    ASSERT_TRUE(foundB);
    ASSERT_TRUE(foundC);
}

TEST_CASE(TestDeadCodeElimination) {
    std::string source = "int a = 10;\nint unused = 50;\nint b = a + 5;";
    auto tac = compileToTAC(source);

    Optimizer optimizer;
    auto optTac = optimizer.optimize(tac);

    bool foundUnused = false;
    for (const auto& instr : optTac.getInstructions()) {
        if (instr.dest == "unused") {
            foundUnused = true;
        }
    }
    ASSERT_TRUE(!foundUnused);
}

TEST_CASE(TestCommonSubexpressionElimination) {
    // Given int a = x + y; int b = x + y;
    // x + y should not be computed twice
    std::string source = "int x = 10;\nint y = 20;\nint a = x + y;\nint b = y + x;\nint res = a + b;";
    auto tac = compileToTAC(source);

    // Run without constant folding to observe CSE specifically on expressions
    OptimizerOptions opts;
    opts.enableConstantFolding = false;
    opts.enableConstantPropagation = false;
    opts.enableDeadCodeElimination = false;
    opts.enableCSE = true;

    Optimizer optimizer(opts);
    auto optTac = optimizer.optimize(tac);

    // Count how many ADD instructions remain for x and y
    int addCount = 0;
    for (const auto& instr : optTac.getInstructions()) {
        if (instr.op == OpCode::ADD &&
            ((instr.src1 == "x" && instr.src2 == "y") || (instr.src1 == "y" && instr.src2 == "x"))) {
            addCount++;
        }
    }
    // With CSE and commutativity, x + y was computed once!
    ASSERT_EQ(addCount, 1);
}

TEST_CASE(TestControlFlowOptimization) {
    std::string source = "int x = 0;\nwhile (x < 10) {\n    x = x + 1;\n}";
    auto tac = compileToTAC(source);

    Optimizer optimizer;
    auto optTac = optimizer.optimize(tac);

    // Verify while loop labels and jumps remain intact and loop is not accidentally removed
    bool hasJumpIfFalse = false;
    bool hasJump = false;
    for (const auto& instr : optTac.getInstructions()) {
        if (instr.op == OpCode::JUMP_IF_FALSE) hasJumpIfFalse = true;
        if (instr.op == OpCode::JUMP) hasJump = true;
    }
    ASSERT_TRUE(hasJumpIfFalse);
    ASSERT_TRUE(hasJump);
}

int main() {
    return printTestSummary("Optimizer Tests");
}
