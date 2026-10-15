#ifndef MINIO_ALGEBRAIC_SIMPLIFICATION_H
#define MINIO_ALGEBRAIC_SIMPLIFICATION_H

#include "ir/TAC.h"
#include <string>

namespace miniopt {

class AlgebraicSimplification {
public:
    // Returns true if at least one instruction was simplified
    bool run(TACProgram& program);

private:
    bool isZero(const std::string& str) const;
    bool isOne(const std::string& str) const;
};

} // namespace miniopt

#endif // MINIO_ALGEBRAIC_SIMPLIFICATION_H
