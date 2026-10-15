#ifndef MINIO_CONSTANT_PROPAGATION_H
#define MINIO_CONSTANT_PROPAGATION_H

#include "ir/TAC.h"
#include <unordered_map>
#include <string>

namespace miniopt {

class ConstantPropagation {
public:
    // Returns true if at least one constant was propagated
    bool run(TACProgram& program);

private:
    bool isInteger(const std::string& str) const;
};

} // namespace miniopt

#endif // MINIO_CONSTANT_PROPAGATION_H
