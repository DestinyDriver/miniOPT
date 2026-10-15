#include "optimizer/AlgebraicSimplification.h"

namespace miniopt {

bool AlgebraicSimplification::isZero(const std::string& str) const {
    return str == "0";
}

bool AlgebraicSimplification::isOne(const std::string& str) const {
    return str == "1";
}

bool AlgebraicSimplification::run(TACProgram& program) {
    bool changed = false;
    auto& instructions = program.getInstructions();

    for (auto& instr : instructions) {
        if (!isArithmeticOp(instr.op)) {
            continue;
        }

        switch (instr.op) {
            case OpCode::ADD:
                // x + 0 -> x
                if (isZero(instr.src2)) {
                    instr.op = OpCode::MOV;
                    // instr.src1 stays the same
                    instr.src2 = "";
                    changed = true;
                }
                // 0 + x -> x
                else if (isZero(instr.src1)) {
                    instr.op = OpCode::MOV;
                    instr.src1 = instr.src2;
                    instr.src2 = "";
                    changed = true;
                }
                break;

            case OpCode::SUB:
                // x - 0 -> x
                if (isZero(instr.src2)) {
                    instr.op = OpCode::MOV;
                    // instr.src1 stays the same
                    instr.src2 = "";
                    changed = true;
                }
                // x - x -> 0
                else if (!instr.src1.empty() && instr.src1 == instr.src2) {
                    instr.op = OpCode::MOV;
                    instr.src1 = "0";
                    instr.src2 = "";
                    changed = true;
                }
                break;

            case OpCode::MUL:
                // x * 1 -> x
                if (isOne(instr.src2)) {
                    instr.op = OpCode::MOV;
                    // instr.src1 stays the same
                    instr.src2 = "";
                    changed = true;
                }
                // 1 * x -> x
                else if (isOne(instr.src1)) {
                    instr.op = OpCode::MOV;
                    instr.src1 = instr.src2;
                    instr.src2 = "";
                    changed = true;
                }
                // x * 0 -> 0
                else if (isZero(instr.src2) || isZero(instr.src1)) {
                    instr.op = OpCode::MOV;
                    instr.src1 = "0";
                    instr.src2 = "";
                    changed = true;
                }
                break;

            case OpCode::DIV:
                // x / 1 -> x
                if (isOne(instr.src2)) {
                    instr.op = OpCode::MOV;
                    // instr.src1 stays the same
                    instr.src2 = "";
                    changed = true;
                }
                // x / x -> 1 (when x is not 0)
                else if (!instr.src1.empty() && instr.src1 == instr.src2 && !isZero(instr.src1)) {
                    instr.op = OpCode::MOV;
                    instr.src1 = "1";
                    instr.src2 = "";
                    changed = true;
                }
                break;

            case OpCode::MOD:
                // x % 1 -> 0
                if (isOne(instr.src2)) {
                    instr.op = OpCode::MOV;
                    instr.src1 = "0";
                    instr.src2 = "";
                    changed = true;
                }
                // x % x -> 0 (when x is not 0)
                else if (!instr.src1.empty() && instr.src1 == instr.src2 && !isZero(instr.src1)) {
                    instr.op = OpCode::MOV;
                    instr.src1 = "0";
                    instr.src2 = "";
                    changed = true;
                }
                break;

            default:
                break;
        }
    }

    return changed;
}

} // namespace miniopt
