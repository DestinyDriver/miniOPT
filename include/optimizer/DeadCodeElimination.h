#ifndef MINIO_DEAD_CODE_ELIMINATION_H
#define MINIO_DEAD_CODE_ELIMINATION_H

#include "ir/TAC.h"
#include <unordered_set>
#include <string>

namespace miniopt {

class DeadCodeElimination {
public:
    // Returns true if at least one dead instruction was eliminated
    bool run(TACProgram& program);

private:
    bool isTemporary(const std::string& name) const;
};

} // namespace miniopt

#endif // MINIO_DEAD_CODE_ELIMINATION_H
