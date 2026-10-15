#include "optimizer/Optimizer.h"
#include "optimizer/ConstantFolding.h"
#include "optimizer/ConstantPropagation.h"
#include "optimizer/AlgebraicSimplification.h"
#include "optimizer/DeadCodeElimination.h"

namespace miniopt {

Optimizer::Optimizer(OptimizerOptions options) : options(options) {}

TACProgram Optimizer::optimize(const TACProgram& input) {
    TACProgram program = input;
    bool changed = true;
    int iterations = 0;
    const int MAX_ITERATIONS = 15;

    ConstantFolding constFolder;
    ConstantPropagation constPropagator;
    AlgebraicSimplification algSimplifier;
    DeadCodeElimination dce;

    while (changed && iterations < MAX_ITERATIONS) {
        changed = false;
        iterations++;

        if (options.enableConstantFolding) {
            if (constFolder.run(program)) {
                changed = true;
            }
        }

        if (options.enableConstantPropagation) {
            if (constPropagator.run(program)) {
                changed = true;
            }
        }

        if (options.enableAlgebraicSimplification) {
            if (algSimplifier.run(program)) {
                changed = true;
            }
        }

        if (options.enableCSE) {
            if (runCommonSubexpressionElimination(program)) {
                changed = true;
            }
        }

        if (options.enableDeadCodeElimination) {
            if (dce.run(program)) {
                changed = true;
            }
        }

        if (runControlFlowOptimization(program)) {
            changed = true;
        }
    }

    return program;
}

namespace {
struct CSEKey {
    OpCode op;
    std::string arg1;
    std::string arg2;

    bool operator==(const CSEKey& other) const {
        return op == other.op && arg1 == other.arg1 && arg2 == other.arg2;
    }
};

struct CSEKeyHash {
    size_t operator()(const CSEKey& k) const {
        return std::hash<int>()(static_cast<int>(k.op)) ^
               (std::hash<std::string>()(k.arg1) << 1) ^
               (std::hash<std::string>()(k.arg2) << 2);
    }
};

CSEKey makeCSEKey(OpCode op, const std::string& a1, const std::string& a2) {
    if (isCommutativeOp(op)) {
        if (a1 <= a2) {
            return {op, a1, a2};
        } else {
            return {op, a2, a1};
        }
    }
    return {op, a1, a2};
}
} // anonymous namespace

bool Optimizer::runCommonSubexpressionElimination(TACProgram& program) {
    bool changed = false;
    auto& instructions = program.getInstructions();

    std::unordered_map<CSEKey, std::string, CSEKeyHash> exprTable;

    for (auto& instr : instructions) {
        // Control flow boundaries reset available expressions
        if (instr.op == OpCode::LABEL || instr.op == OpCode::JUMP || instr.op == OpCode::JUMP_IF_FALSE) {
            exprTable.clear();
            continue;
        }

        if (isArithmeticOp(instr.op) || isComparisonOp(instr.op)) {
            CSEKey key = makeCSEKey(instr.op, instr.src1, instr.src2);
            auto it = exprTable.find(key);

            if (it != exprTable.end()) {
                // Common subexpression found! Reuse previously computed variable
                instr.op = OpCode::MOV;
                instr.src1 = it->second;
                instr.src2 = "";
                changed = true;
            } else {
                // Record this expression
                exprTable[key] = instr.dest;
            }
        }

        // When a variable is defined / reassigned, invalidate any cached expressions that depend on it
        if (!instr.dest.empty()) {
            std::string writtenVar = instr.dest;
            for (auto it = exprTable.begin(); it != exprTable.end();) {
                if (it->first.arg1 == writtenVar || it->first.arg2 == writtenVar) {
                    it = exprTable.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    return changed;
}

bool Optimizer::runControlFlowOptimization(TACProgram& program) {
    bool changed = false;
    auto& instructions = program.getInstructions();
    std::vector<TACInstruction> result;
    result.reserve(instructions.size());

    bool unreachable = false;

    for (size_t i = 0; i < instructions.size(); ++i) {
        auto instr = instructions[i];

        // Labels always restore reachability
        if (instr.op == OpCode::LABEL) {
            unreachable = false;
            result.push_back(instr);
            continue;
        }

        // If currently in unreachable code block, drop instruction
        if (unreachable) {
            changed = true;
            continue;
        }

        // Constant conditional branch simplification
        if (instr.op == OpCode::JUMP_IF_FALSE) {
            // if_false 1 goto L -> condition is always true, branch is NEVER taken
            if (instr.src1 == "1") {
                changed = true;
                continue; // eliminate dead branch
            }
            // if_false 0 goto L -> condition is always false, branch is ALWAYS taken
            if (instr.src1 == "0") {
                instr.op = OpCode::JUMP;
                instr.dest = instr.dest; // target label is in dest
                instr.src1 = "";
                changed = true;
                // continues to process as JUMP below
            }
        }

        // Redundant jump elimination: goto L immediately before L:
        if (instr.op == OpCode::JUMP) {
            if (i + 1 < instructions.size() && instructions[i + 1].op == OpCode::LABEL &&
                instructions[i + 1].dest == instr.dest) {
                changed = true;
                continue; // drop redundant jump directly before target label
            }
            result.push_back(instr);
            unreachable = true; // subsequent instructions until next label are unreachable
            continue;
        }

        result.push_back(instr);
    }

    if (changed) {
        instructions = std::move(result);
    }
    return changed;
}

} // namespace miniopt
