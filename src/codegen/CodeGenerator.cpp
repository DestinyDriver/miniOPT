#include "codegen/CodeGenerator.h"
#include <sstream>
#include <iomanip>

namespace miniopt {

std::string AsmInstruction::toString() const {
    std::ostringstream oss;
    if (opcode == "LABEL") {
        oss << arg1 << ":";
    } else {
        oss << "    " << std::left << std::setw(14) << opcode;
        if (!arg1.empty()) {
            oss << arg1;
            if (!arg2.empty()) {
                oss << ", " << arg2;
            }
        }
    }
    if (!comment.empty()) {
        oss << "    # " << comment;
    }
    return oss.str();
}

void CodeGenerator::emit(std::vector<AsmInstruction>& code, std::string op, std::string a1, std::string a2, std::string comment) {
    code.emplace_back(std::move(op), std::move(a1), std::move(a2), std::move(comment));
}

std::vector<AsmInstruction> CodeGenerator::generate(const TACProgram& tac) {
    std::vector<AsmInstruction> asmCode;

    for (const auto& instr : tac.getInstructions()) {
        std::string tacComment = instr.toString();
        // Trim leading spaces from TAC string for comments
        size_t firstNonSpace = tacComment.find_first_not_of(" \t");
        if (firstNonSpace != std::string::npos) {
            tacComment = tacComment.substr(firstNonSpace);
        }

        switch (instr.op) {
            case OpCode::LABEL:
                emit(asmCode, "LABEL", instr.dest, "", "");
                break;

            case OpCode::MOV:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::ADD:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "LOAD", "R2", instr.src2, "");
                emit(asmCode, "ADD", "R1", "R2", "");
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::SUB:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "LOAD", "R2", instr.src2, "");
                emit(asmCode, "SUB", "R1", "R2", "");
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::MUL:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "LOAD", "R2", instr.src2, "");
                emit(asmCode, "MUL", "R1", "R2", "");
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::DIV:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "LOAD", "R2", instr.src2, "");
                emit(asmCode, "DIV", "R1", "R2", "");
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::MOD:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "LOAD", "R2", instr.src2, "");
                emit(asmCode, "MOD", "R1", "R2", "");
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::CMP:
            case OpCode::CMP_EQ:
            case OpCode::CMP_NE:
            case OpCode::CMP_LT:
            case OpCode::CMP_GT:
            case OpCode::CMP_LE:
            case OpCode::CMP_GE:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "LOAD", "R2", instr.src2, "");
                emit(asmCode, "CMP", "R1", "R2", "");
                emit(asmCode, "STORE", instr.dest, "R1", "");
                break;

            case OpCode::JUMP:
                emit(asmCode, "JMP", instr.dest, "", tacComment);
                break;

            case OpCode::JUMP_IF_FALSE:
                emit(asmCode, "LOAD", "R1", instr.src1, tacComment);
                emit(asmCode, "JMP_IF_FALSE", "R1", instr.dest, "");
                break;
        }
    }

    return asmCode;
}

std::string CodeGenerator::generateString(const TACProgram& tac) {
    auto instructions = generate(tac);
    std::ostringstream oss;
    for (const auto& instr : instructions) {
        oss << instr.toString() << "\n";
    }
    return oss.str();
}

} // namespace miniopt
