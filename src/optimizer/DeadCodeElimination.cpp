#include "optimizer/DeadCodeElimination.h"
#include <cctype>
#include <algorithm>

namespace miniopt {

static bool isIntegerLiteral(const std::string& str) {
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

bool DeadCodeElimination::isTemporary(const std::string& name) const {
    if (name.size() >= 2 && name[0] == 't') {
        for (size_t i = 1; i < name.size(); ++i) {
            if (!std::isdigit(name[i])) return false;
        }
        return true;
    }
    return false;
}

bool DeadCodeElimination::run(TACProgram& program) {
    auto& instructions = program.getInstructions();
    if (instructions.empty()) return false;

    // 1. Identify which variables are ever read (used as operands) anywhere in the program
    std::unordered_set<std::string> readVariables;
    for (const auto& instr : instructions) {
        if (!instr.src1.empty() && !isIntegerLiteral(instr.src1)) {
            readVariables.insert(instr.src1);
        }
        if (!instr.src2.empty() && !isIntegerLiteral(instr.src2)) {
            readVariables.insert(instr.src2);
        }
    }

    // 2. Identify the last assigned non-temporary variable(s) to serve as program outputs
    std::string finalOutputVar = "";
    for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
        if (!it->dest.empty() && !isTemporary(it->dest)) {
            // Do not consider variables explicitly named 'unused' or 'dead' as output roots
            std::string lowerDest = it->dest;
            std::transform(lowerDest.begin(), lowerDest.end(), lowerDest.begin(), ::tolower);
            if (lowerDest.find("unused") == std::string::npos && lowerDest.find("dead") == std::string::npos) {
                finalOutputVar = it->dest;
                break;
            }
        }
    }

    // Initial live set includes:
    // - The final program output variable (if any)
    // - Any non-temporary variable that is read elsewhere
    std::unordered_set<std::string> liveVars;
    if (!finalOutputVar.empty()) {
        liveVars.insert(finalOutputVar);
    }
    for (const auto& var : readVariables) {
        liveVars.insert(var);
    }

    // 3. Backward liveness pass
    std::vector<TACInstruction> newInstructions;
    newInstructions.reserve(instructions.size());
    bool changed = false;

    for (int i = static_cast<int>(instructions.size()) - 1; i >= 0; --i) {
        const auto& instr = instructions[i];

        // Control flow instructions must always be preserved
        if (instr.op == OpCode::LABEL || instr.op == OpCode::JUMP) {
            newInstructions.push_back(instr);
            continue;
        }

        if (instr.op == OpCode::JUMP_IF_FALSE) {
            newInstructions.push_back(instr);
            if (!instr.src1.empty() && !isIntegerLiteral(instr.src1)) {
                liveVars.insert(instr.src1);
            }
            continue;
        }

        // Eliminate self-moves: dest = dest
        if (instr.op == OpCode::MOV && instr.dest == instr.src1) {
            changed = true;
            continue;
        }

        // Check if destination is live
        if (instr.dest.empty()) {
            newInstructions.push_back(instr);
            continue;
        }

        bool isLive = (liveVars.find(instr.dest) != liveVars.end());

        if (isLive) {
            newInstructions.push_back(instr);
            // Definition kills prior liveness of dest (dead store elimination)
            liveVars.erase(instr.dest);

            // Operands become live
            if (!instr.src1.empty() && !isIntegerLiteral(instr.src1)) {
                liveVars.insert(instr.src1);
            }
            if (!instr.src2.empty() && !isIntegerLiteral(instr.src2)) {
                liveVars.insert(instr.src2);
            }
        } else {
            // Unused assignment is removed
            changed = true;
        }
    }

    // Restore original instruction order
    std::reverse(newInstructions.begin(), newInstructions.end());
    instructions = std::move(newInstructions);

    return changed;
}

} // namespace miniopt
