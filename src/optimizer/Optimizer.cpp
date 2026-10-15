#include "optimizer/Optimizer.h"
#include "optimizer/ConstantFolding.h"
#include "optimizer/AlgebraicSimplification.h"

namespace miniopt {

Optimizer::Optimizer(OptimizerOptions options) : options(options) {}

TACProgram Optimizer::optimize(const TACProgram& input) {
    TACProgram program = input;
    bool changed = true;
    int iterations = 0;
    const int MAX_ITERATIONS = 15;

    ConstantFolding constFolder;
    AlgebraicSimplification algSimplifier;

    while (changed && iterations < MAX_ITERATIONS) {
        changed = false;
        iterations++;

        if (options.enableConstantFolding) {
            if (constFolder.run(program)) {
                changed = true;
            }
        }

        if (options.enableAlgebraicSimplification) {
            if (algSimplifier.run(program)) {
                changed = true;
            }
        }
    }

    return program;
}

} // namespace miniopt
