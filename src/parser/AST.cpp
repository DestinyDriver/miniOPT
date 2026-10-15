#include "parser/AST.h"
#include <sstream>

namespace miniopt {

static std::string makeIndent(int indent) {
    return std::string(indent * 2, ' ');
}

std::string binaryOpToString(BinaryOp op) {
    switch (op) {
        case BinaryOp::ADD:           return "+";
        case BinaryOp::SUB:           return "-";
        case BinaryOp::MUL:           return "*";
        case BinaryOp::DIV:           return "/";
        case BinaryOp::MOD:           return "%";
        case BinaryOp::LESS:          return "<";
        case BinaryOp::GREATER:       return ">";
        case BinaryOp::LESS_EQUAL:    return "<=";
        case BinaryOp::GREATER_EQUAL: return ">=";
        case BinaryOp::EQUAL_EQUAL:   return "==";
        case BinaryOp::NOT_EQUAL:     return "!=";
        default:                      return "?";
    }
}

std::string ASTNode::toString() const {
    std::ostringstream oss;
    print(oss, 0);
    return oss.str();
}

void NumberExpr::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "NumberExpr(" << value << ")\n";
}

void VariableExpr::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "VariableExpr(" << name << ")\n";
}

void BinaryExpr::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "BinaryExpr (" << binaryOpToString(op) << ")\n";
    if (left) {
        left->print(os, indent + 1);
    }
    if (right) {
        right->print(os, indent + 1);
    }
}

void VariableDeclaration::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "VariableDeclaration: " << type << " " << name << "\n";
    if (initializer) {
        initializer->print(os, indent + 1);
    }
}

void Assignment::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "Assignment: " << name << "\n";
    if (value) {
        value->print(os, indent + 1);
    }
}

void BlockStatement::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "Block {\n";
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->print(os, indent + 1);
        }
    }
    os << makeIndent(indent) << "}\n";
}

void IfStatement::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "IfStatement\n";
    os << makeIndent(indent + 1) << "Condition:\n";
    if (condition) {
        condition->print(os, indent + 2);
    }
    os << makeIndent(indent + 1) << "Then:\n";
    if (thenBranch) {
        thenBranch->print(os, indent + 2);
    }
}

void WhileStatement::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "WhileStatement\n";
    os << makeIndent(indent + 1) << "Condition:\n";
    if (condition) {
        condition->print(os, indent + 2);
    }
    os << makeIndent(indent + 1) << "Body:\n";
    if (body) {
        body->print(os, indent + 2);
    }
}

void Program::print(std::ostream& os, int indent) const {
    os << makeIndent(indent) << "Program\n";
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->print(os, indent + 1);
        }
    }
}

} // namespace miniopt
