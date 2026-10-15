#ifndef MINIO_TAC_H
#define MINIO_TAC_H

#include <string>
#include <vector>
#include <iostream>

namespace miniopt {

enum class OpCode {
    ADD,            // dest = src1 + src2
    SUB,            // dest = src1 - src2
    MUL,            // dest = src1 * src2
    DIV,            // dest = src1 / src2
    MOD,            // dest = src1 % src2
    MOV,            // dest = src1
    CMP,            // dest = src1 cmp src2
    CMP_EQ,         // dest = src1 == src2
    CMP_NE,         // dest = src1 != src2
    CMP_LT,         // dest = src1 < src2
    CMP_GT,         // dest = src1 > src2
    CMP_LE,         // dest = src1 <= src2
    CMP_GE,         // dest = src1 >= src2
    JUMP,           // goto dest
    JUMP_IF_FALSE,  // if_false src1 goto dest
    LABEL           // dest:
};

std::string opCodeToString(OpCode op);

inline std::ostream& operator<<(std::ostream& os, OpCode op) {
    return os << opCodeToString(op);
}
bool isArithmeticOp(OpCode op);
bool isComparisonOp(OpCode op);
bool isCommutativeOp(OpCode op);

struct TACInstruction {
    OpCode op;
    std::string dest;
    std::string src1;
    std::string src2;

    TACInstruction(OpCode op, std::string dest, std::string src1 = "", std::string src2 = "")
        : op(op), dest(std::move(dest)), src1(std::move(src1)), src2(std::move(src2)) {}

    std::string toString() const;
    bool operator==(const TACInstruction& other) const {
        return op == other.op && dest == other.dest && src1 == other.src1 && src2 == other.src2;
    }
};

class TACProgram {
public:
    void addInstruction(TACInstruction instr);
    void addInstruction(OpCode op, std::string dest, std::string src1 = "", std::string src2 = "");

    const std::vector<TACInstruction>& getInstructions() const { return instructions; }
    std::vector<TACInstruction>& getInstructions() { return instructions; }
    void setInstructions(std::vector<TACInstruction> instrs) { instructions = std::move(instrs); }

    size_t size() const { return instructions.size(); }
    void print(std::ostream& os) const;
    std::string toString() const;

private:
    std::vector<TACInstruction> instructions;
};

} // namespace miniopt

#endif // MINIO_TAC_H
