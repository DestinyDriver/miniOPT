#include "ir/IRGenerator.h"

namespace miniopt {

IRGenerator::IRGenerator() : tempCounter(1), labelCounter(1) {}

void IRGenerator::reset() {
    tempCounter = 1;
    labelCounter = 1;
    declaredVariables.clear();
}

std::string IRGenerator::newTemp() {
    return "t" + std::to_string(tempCounter++);
}

std::string IRGenerator::newLabel() {
    return "L" + std::to_string(labelCounter++);
}

TACProgram IRGenerator::generate(const Program& program) {
    reset();
    TACProgram tac;
    for (const auto& stmt : program.getStatements()) {
        if (stmt) {
            generateStatement(stmt.get(), tac);
        }
    }
    return tac;
}

std::string IRGenerator::generateExpression(const Expression* expr, TACProgram& tac) {
    if (!expr) {
        return "";
    }

    if (auto num = dynamic_cast<const NumberExpr*>(expr)) {
        return std::to_string(num->getValue());
    }

    if (auto var = dynamic_cast<const VariableExpr*>(expr)) {
        if (declaredVariables.find(var->getName()) == declaredVariables.end()) {
            throw IRError("Undefined variable '" + var->getName() + "'");
        }
        return var->getName();
    }

    if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
        std::string left = generateExpression(bin->getLeft(), tac);
        std::string right = generateExpression(bin->getRight(), tac);

        OpCode op;
        switch (bin->getOp()) {
            case BinaryOp::ADD:           op = OpCode::ADD; break;
            case BinaryOp::SUB:           op = OpCode::SUB; break;
            case BinaryOp::MUL:           op = OpCode::MUL; break;
            case BinaryOp::DIV:           op = OpCode::DIV; break;
            case BinaryOp::MOD:           op = OpCode::MOD; break;
            case BinaryOp::LESS:          op = OpCode::CMP_LT; break;
            case BinaryOp::GREATER:       op = OpCode::CMP_GT; break;
            case BinaryOp::LESS_EQUAL:    op = OpCode::CMP_LE; break;
            case BinaryOp::GREATER_EQUAL: op = OpCode::CMP_GE; break;
            case BinaryOp::EQUAL_EQUAL:   op = OpCode::CMP_EQ; break;
            case BinaryOp::NOT_EQUAL:     op = OpCode::CMP_NE; break;
            default:                      op = OpCode::ADD; break;
        }

        std::string dest = newTemp();
        tac.addInstruction(op, dest, left, right);
        return dest;
    }

    throw IRError("Unknown expression type encountered in IR generation");
}

void IRGenerator::generateStatement(const Statement* stmt, TACProgram& tac) {
    if (!stmt) return;

    if (auto decl = dynamic_cast<const VariableDeclaration*>(stmt)) {
        declaredVariables.insert(decl->getName());
        if (decl->getInitializer()) {
            std::string src = generateExpression(decl->getInitializer(), tac);
            tac.addInstruction(OpCode::MOV, decl->getName(), src);
        }
        return;
    }

    if (auto assign = dynamic_cast<const Assignment*>(stmt)) {
        if (declaredVariables.find(assign->getName()) == declaredVariables.end()) {
            throw IRError("Undefined variable '" + assign->getName() + "'");
        }
        std::string src = generateExpression(assign->getValue(), tac);
        tac.addInstruction(OpCode::MOV, assign->getName(), src);
        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStatement*>(stmt)) {
        std::string cond = generateExpression(ifStmt->getCondition(), tac);
        std::string endLabel = newLabel();
        tac.addInstruction(OpCode::JUMP_IF_FALSE, endLabel, cond);
        if (ifStmt->getThenBranch()) {
            generateBlock(ifStmt->getThenBranch(), tac);
        }
        tac.addInstruction(OpCode::LABEL, endLabel);
        return;
    }

    if (auto whileStmt = dynamic_cast<const WhileStatement*>(stmt)) {
        std::string startLabel = newLabel();
        std::string endLabel = newLabel();
        tac.addInstruction(OpCode::LABEL, startLabel);
        std::string cond = generateExpression(whileStmt->getCondition(), tac);
        tac.addInstruction(OpCode::JUMP_IF_FALSE, endLabel, cond);
        if (whileStmt->getBody()) {
            generateBlock(whileStmt->getBody(), tac);
        }
        tac.addInstruction(OpCode::JUMP, startLabel);
        tac.addInstruction(OpCode::LABEL, endLabel);
        return;
    }

    if (auto block = dynamic_cast<const BlockStatement*>(stmt)) {
        generateBlock(block, tac);
        return;
    }

    throw IRError("Unknown statement type encountered in IR generation");
}

void IRGenerator::generateBlock(const BlockStatement* block, TACProgram& tac) {
    if (!block) return;
    for (const auto& stmt : block->getStatements()) {
        if (stmt) {
            generateStatement(stmt.get(), tac);
        }
    }
}

} // namespace miniopt
