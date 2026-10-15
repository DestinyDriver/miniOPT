#ifndef MINIO_AST_H
#define MINIO_AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace miniopt {

enum class BinaryOp {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL
};

std::string binaryOpToString(BinaryOp op);

inline std::ostream& operator<<(std::ostream& os, BinaryOp op) {
    return os << binaryOpToString(op);
}

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(std::ostream& os, int indent = 0) const = 0;
    std::string toString() const;
};

// Base Expression Node
class Expression : public ASTNode {
public:
    ~Expression() override = default;
};

// Integer Literal Expression
class NumberExpr : public Expression {
public:
    explicit NumberExpr(int value) : value(value) {}
    int getValue() const { return value; }

    void print(std::ostream& os, int indent = 0) const override;

private:
    int value;
};

// Variable Reference Expression
class VariableExpr : public Expression {
public:
    explicit VariableExpr(std::string name) : name(std::move(name)) {}
    const std::string& getName() const { return name; }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::string name;
};

// Binary Operation Expression
class BinaryExpr : public Expression {
public:
    BinaryExpr(BinaryOp op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    BinaryOp getOp() const { return op; }
    const Expression* getLeft() const { return left.get(); }
    const Expression* getRight() const { return right.get(); }
    Expression* getLeft() { return left.get(); }
    Expression* getRight() { return right.get(); }

    void print(std::ostream& os, int indent = 0) const override;

private:
    BinaryOp op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

// Base Statement Node
class Statement : public ASTNode {
public:
    ~Statement() override = default;
};

// Variable Declaration Statement: int x = expr;
class VariableDeclaration : public Statement {
public:
    VariableDeclaration(std::string type, std::string name, std::unique_ptr<Expression> init)
        : type(std::move(type)), name(std::move(name)), initializer(std::move(init)) {}

    const std::string& getType() const { return type; }
    const std::string& getName() const { return name; }
    const Expression* getInitializer() const { return initializer.get(); }
    Expression* getInitializer() { return initializer.get(); }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::string type;
    std::string name;
    std::unique_ptr<Expression> initializer;
};

// Assignment Statement: x = expr;
class Assignment : public Statement {
public:
    Assignment(std::string name, std::unique_ptr<Expression> value)
        : name(std::move(name)), value(std::move(value)) {}

    const std::string& getName() const { return name; }
    const Expression* getValue() const { return value.get(); }
    Expression* getValue() { return value.get(); }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::string name;
    std::unique_ptr<Expression> value;
};

// Block Statement: { stmt* }
class BlockStatement : public Statement {
public:
    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}

    const std::vector<std::unique_ptr<Statement>>& getStatements() const { return statements; }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::vector<std::unique_ptr<Statement>> statements;
};

// If Statement: if (cond) { ... }
class IfStatement : public Statement {
public:
    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockStatement> thenBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)) {}

    const Expression* getCondition() const { return condition.get(); }
    const BlockStatement* getThenBranch() const { return thenBranch.get(); }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> thenBranch;
};

// While Statement: while (cond) { ... }
class WhileStatement : public Statement {
public:
    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockStatement> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    const Expression* getCondition() const { return condition.get(); }
    const BlockStatement* getBody() const { return body.get(); }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;
};

// Root Program Node
class Program : public ASTNode {
public:
    explicit Program(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements)) {}

    const std::vector<std::unique_ptr<Statement>>& getStatements() const { return statements; }

    void print(std::ostream& os, int indent = 0) const override;

private:
    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace miniopt

#endif // MINIO_AST_H
