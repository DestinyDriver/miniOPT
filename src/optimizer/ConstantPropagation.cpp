#include "optimizer/ConstantPropagation.h"
#include <cctype>

namespace miniopt {

bool ConstantPropagation::isInteger(const std::string& str) const {
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

bool ConstantPropagation::run(TACProgram& program) {
    bool changed = false;
    auto& instructions = program.getInstructions();
    std::unordered_map<std::string, std::string> constMap;

    for (auto& instr : instructions) {
        // Control flow boundaries invalidate the local forward constant state
        if (instr.op == OpCode::LABEL || instr.op == OpCode::JUMP || instr.op == OpCode::JUMP_IF_FALSE) {
            if (instr.op == OpCode::JUMP_IF_FALSE) {
                // If the jump condition itself is a known constant, propagate it first
                auto it = constMap.find(instr.src1);
                if (it != constMap.end()) {
                    instr.src1 = it->second;
                    changed = true;
                }
            }
            constMap.clear();
            continue;
        }

        // Propagate known constant to src1
        if (!instr.src1.empty() && !isInteger(instr.src1)) {
            auto it = constMap.find(instr.src1);
            if (it != constMap.end()) {
                instr.src1 = it->second;
                changed = true;
            }
        }

        // Propagate known constant to src2
        if (!instr.src2.empty() && !isInteger(instr.src2)) {
            auto it = constMap.find(instr.src2);
            if (it != constMap.end()) {
                instr.src2 = it->second;
                changed = true;
            }
        }

        // Update constant map for dest
        if (!instr.dest.empty()) {
            if (instr.op == OpCode::MOV && isInteger(instr.src1)) {
                constMap[instr.dest] = instr.src1;
            } else {
                constMap.erase(instr.dest);
            }
        }
    }

    return changed;
}

} // namespace miniopt
