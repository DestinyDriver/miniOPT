#ifndef MINIO_CONSTANT_FOLDING_H
#define MINIO_CONSTANT_FOLDING_H

#include "ir/TAC.h"
#include <string>

namespace miniopt {

class ConstantFolding {
public:
    // Returns true if at least one instruction was folded
    bool run(TACProgram& program);

private:
    bool isInteger(const std::string& str) const;
    int parseInt(const std::string& str) const;
};

} // namespace miniopt

#endif // MINIO_CONSTANT_FOLDING_H
