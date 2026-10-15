#ifndef MINIO_OPTIMIZER_H
#define MINIO_OPTIMIZER_H

#include "ir/TAC.h"
#include <string>

namespace miniopt {

struct OptimizerOptions {
    bool enableConstantFolding = true;
    bool enableConstantPropagation = true;
    bool enableAlgebraicSimplification = true;
    bool enableCSE = true;
    bool enableDeadCodeElimination = true;
};

class Optimizer {
public:
    explicit Optimizer(OptimizerOptions options = OptimizerOptions{});

    TACProgram optimize(const TACProgram& input);

    const OptimizerOptions& getOptions() const { return options; }
    void setOptions(const OptimizerOptions& opts) { options = opts; }

    bool runCommonSubexpressionElimination(TACProgram& program);
    bool runControlFlowOptimization(TACProgram& program);

private:
    OptimizerOptions options;
};

} // namespace miniopt

#endif // MINIO_OPTIMIZER_H
