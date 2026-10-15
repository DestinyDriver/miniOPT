#ifndef MINIO_IR_GENERATOR_H
#define MINIO_IR_GENERATOR_H

#include "parser/AST.h"
#include "ir/TAC.h"
#include <unordered_set>
#include <string>
#include <stdexcept>

namespace miniopt {

class IRError : public std::runtime_error {
public:
    explicit IRError(const std::string& message)
        : std::runtime_error("IR Generation Error: " + message) {}
};

class IRGenerator {
public:
    IRGenerator();

    TACProgram generate(const Program& program);

    void reset();

private:
    std::string newTemp();
    std::string newLabel();

    std::string generateExpression(const Expression* expr, TACProgram& tac);
    void generateStatement(const Statement* stmt, TACProgram& tac);
    void generateBlock(const BlockStatement* block, TACProgram& tac);

    int tempCounter;
    int labelCounter;
    std::unordered_set<std::string> declaredVariables;
};

} // namespace miniopt

#endif // MINIO_IR_GENERATOR_H
