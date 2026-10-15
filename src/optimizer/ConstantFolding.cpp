#include "optimizer/ConstantFolding.h"
#include <cctype>
#include <cstdlib>

namespace miniopt {

bool ConstantFolding::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    size_t i = 0;
    if (str[0] == '-' || str[0] == '+') {
        if (str.size() == 1) return false;
        i = 1;
    }
    for (; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    return true;
}

int ConstantFolding::parseInt(const std::string& str) const {
    return std::stoi(str);
}

bool ConstantFolding::run(TACProgram& program) {
    bool changed = false;
    auto& instructions = program.getInstructions();

    for (auto& instr : instructions) {
        if (!isArithmeticOp(instr.op) && !isComparisonOp(instr.op)) {
            continue;
        }

        if (isInteger(instr.src1) && isInteger(instr.src2)) {
            int v1 = parseInt(instr.src1);
            int v2 = parseInt(instr.src2);
            int result = 0;
            bool valid = true;

            switch (instr.op) {
                case OpCode::ADD:
                    result = v1 + v2;
                    break;
                case OpCode::SUB:
                    result = v1 - v2;
                    break;
                case OpCode::MUL:
                    result = v1 * v2;
                    break;
                case OpCode::DIV:
                    if (v2 == 0) {
                        valid = false; // Avoid division by zero at compile time
                    } else {
                        result = v1 / v2;
                    }
                    break;
                case OpCode::MOD:
                    if (v2 == 0) {
                        valid = false; // Avoid modulo by zero at compile time
                    } else {
                        result = v1 % v2;
                    }
                    break;
                case OpCode::CMP_LT:
                    result = (v1 < v2) ? 1 : 0;
                    break;
                case OpCode::CMP_GT:
                    result = (v1 > v2) ? 1 : 0;
                    break;
                case OpCode::CMP_LE:
                    result = (v1 <= v2) ? 1 : 0;
                    break;
                case OpCode::CMP_GE:
                    result = (v1 >= v2) ? 1 : 0;
                    break;
                case OpCode::CMP_EQ:
                    result = (v1 == v2) ? 1 : 0;
                    break;
                case OpCode::CMP_NE:
                    result = (v1 != v2) ? 1 : 0;
                    break;
                default:
                    valid = false;
                    break;
            }

            if (valid) {
                instr.op = OpCode::MOV;
                instr.src1 = std::to_string(result);
                instr.src2 = "";
                changed = true;
            }
        }
    }

    return changed;
}

} // namespace miniopt
