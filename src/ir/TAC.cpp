#include "ir/TAC.h"
#include <sstream>

namespace miniopt {

std::string opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::ADD:           return "+";
        case OpCode::SUB:           return "-";
        case OpCode::MUL:           return "*";
        case OpCode::DIV:           return "/";
        case OpCode::MOD:           return "%";
        case OpCode::MOV:           return "MOV";
        case OpCode::CMP:           return "CMP";
        case OpCode::CMP_EQ:        return "==";
        case OpCode::CMP_NE:        return "!=";
        case OpCode::CMP_LT:        return "<";
        case OpCode::CMP_GT:        return ">";
        case OpCode::CMP_LE:        return "<=";
        case OpCode::CMP_GE:        return ">=";
        case OpCode::JUMP:          return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::LABEL:         return "LABEL";
        default:                    return "?";
    }
}

bool isArithmeticOp(OpCode op) {
    return op == OpCode::ADD || op == OpCode::SUB || op == OpCode::MUL ||
           op == OpCode::DIV || op == OpCode::MOD;
}

bool isComparisonOp(OpCode op) {
    return op == OpCode::CMP || op == OpCode::CMP_EQ || op == OpCode::CMP_NE ||
           op == OpCode::CMP_LT || op == OpCode::CMP_GT || op == OpCode::CMP_LE ||
           op == OpCode::CMP_GE;
}

bool isCommutativeOp(OpCode op) {
    return op == OpCode::ADD || op == OpCode::MUL ||
           op == OpCode::CMP_EQ || op == OpCode::CMP_NE;
}

std::string TACInstruction::toString() const {
    std::ostringstream oss;
    switch (op) {
        case OpCode::LABEL:
            oss << dest << ":";
            break;
        case OpCode::MOV:
            oss << "  " << dest << " = " << src1;
            break;
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        case OpCode::MOD:
            oss << "  " << dest << " = " << src1 << " " << opCodeToString(op) << " " << src2;
            break;
        case OpCode::CMP:
            oss << "  " << dest << " = " << src1 << " CMP " << src2;
            break;
        case OpCode::CMP_EQ:
        case OpCode::CMP_NE:
        case OpCode::CMP_LT:
        case OpCode::CMP_GT:
        case OpCode::CMP_LE:
        case OpCode::CMP_GE:
            oss << "  " << dest << " = " << src1 << " " << opCodeToString(op) << " " << src2;
            break;
        case OpCode::JUMP:
            oss << "  goto " << dest;
            break;
        case OpCode::JUMP_IF_FALSE:
            oss << "  if_false " << src1 << " goto " << dest;
            break;
    }
    return oss.str();
}

void TACProgram::addInstruction(TACInstruction instr) {
    instructions.push_back(std::move(instr));
}

void TACProgram::addInstruction(OpCode op, std::string dest, std::string src1, std::string src2) {
    instructions.emplace_back(op, std::move(dest), std::move(src1), std::move(src2));
}

void TACProgram::print(std::ostream& os) const {
    for (const auto& instr : instructions) {
        os << instr.toString() << "\n";
    }
}

std::string TACProgram::toString() const {
    std::ostringstream oss;
    print(oss);
    return oss.str();
}

} // namespace miniopt
