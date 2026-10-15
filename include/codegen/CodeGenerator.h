#ifndef MINIO_CODE_GENERATOR_H
#define MINIO_CODE_GENERATOR_H

#include "ir/TAC.h"
#include <string>
#include <vector>
#include <iostream>

namespace miniopt {

struct AsmInstruction {
    std::string opcode;
    std::string arg1;
    std::string arg2;
    std::string comment;

    AsmInstruction(std::string op, std::string a1 = "", std::string a2 = "", std::string comment = "")
        : opcode(std::move(op)), arg1(std::move(a1)), arg2(std::move(a2)), comment(std::move(comment)) {}

    std::string toString() const;
};

class CodeGenerator {
public:
    std::vector<AsmInstruction> generate(const TACProgram& tac);
    std::string generateString(const TACProgram& tac);

private:
    void emit(std::vector<AsmInstruction>& code, std::string op, std::string a1 = "", std::string a2 = "", std::string comment = "");
};

} // namespace miniopt

#endif // MINIO_CODE_GENERATOR_H
