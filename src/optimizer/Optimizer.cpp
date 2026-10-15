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

        if (options.enableDeadCodeElimination) {
            if (dce.run(program)) {
                changed = true;
            }
        }
    }

    return program;
}

} // namespace miniopt
