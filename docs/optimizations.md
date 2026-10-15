# MiniOpt Compiler Optimization Passes

This document details the optimization passes implemented in **MiniOpt**, explaining their algorithmic logic, correctness invariants, and before-and-after transformations.

---

## 1. Pipeline Execution Model

MiniOpt applies optimizations in a carefully ordered pipeline wrapped in an **iterative fixed-point loop**:

```text
TAC Program
   │
   ├──> 1. Constant Folding
   │       └── Evaluates compile-time constant arithmetic and comparisons
   │
   ├──> 2. Constant Propagation
   │       └── Propagates known constants into subsequent operand uses
   │
   ├──> 3. Algebraic Simplification
   │       └── Applies algebraic identity and annihilation laws
   │
   ├──> 4. Common Subexpression Elimination (CSE)
   │       └── Eliminates redundant recomputations with commutativity support
   │
   ├──> 5. Dead Code Elimination (DCE)
   │       └── Backward liveness analysis removing unused instructions
   │
   ├──> 6. Control Flow Optimization
   │       └── Prunes dead branches and removes unreachable code blocks
   │
   └── Repeat until no pass produces modifications (or max iterations reached)
```

The fixed-point loop is essential because optimizations exhibit **synergistic cascading effects**:
- Folding `10 + 20` enables Constant Propagation of `30`.
- Propagating `30` into `x * 0` enables Algebraic Simplification to `0`.
- Simplifying expressions leaves temporary variables unread, allowing Dead Code Elimination to prune them.

---

## 2. Optimization Passes in Detail

### 2.1 Constant Folding (`ConstantFolding.h`, `ConstantFolding.cpp`)

#### Algorithm
Scans all arithmetic (`+`, `-`, `*`, `/`, `%`) and comparison (`<`, `>`, `<=`, `>=`, `==`, `!=`) instructions. If both `src1` and `src2` are integer literals, the compiler precomputes the result and replaces the instruction with a direct assignment `dest = result`.

#### Safety Constraints
- **Division / Modulo by Zero**: If `src2 == "0"` for `/` or `%`, folding is aborted, preserving the runtime fault behavior rather than crashing the compiler.

#### Transformation Example
```text
# Before Constant Folding
t1 = 20 * 2
t2 = 10 + t1
t3 = 100 / 4

# After Constant Folding
t1 = 40
t2 = 10 + t1
t3 = 25
```

---

### 2.2 Constant Propagation (`ConstantPropagation.h`, `ConstantPropagation.cpp`)

#### Algorithm
Maintains a forward symbol mapping:
$$\text{constMap} : \text{Variable} \to \text{Constant Literal}$$

1. For each instruction, if `src1` or `src2` exists in `constMap`, replaces the operand with the known constant value.
2. If the instruction assigns a constant literal (`dest = <number>`), updates $\text{constMap}[\text{dest}] \leftarrow \text{number}$.
3. If `dest` is assigned a non-constant expression, `dest` is invalidated ($\text{constMap}.\text{erase}(\text{dest})$).
4. Upon encountering any control-flow boundary (`LABEL`, `JUMP`, `JUMP_IF_FALSE`), $\text{constMap}$ is conservatively cleared to prevent incorrect cross-block propagation.

#### Transformation Example
```text
# Before Constant Propagation
a = 10
b = a + 5
c = b * 2

# After Constant Propagation + Constant Folding
a = 10
b = 15
c = 30
```

---

### 2.3 Algebraic Simplification (`AlgebraicSimplification.h`, `AlgebraicSimplification.cpp`)

#### Algorithm
Applies mathematical identities to simplify expressions without changing program semantics:

| Pattern | Transformation | Rationale |
| :--- | :--- | :--- |
| `x + 0` or `0 + x` | `MOV dest, x` | Additive identity |
| `x - 0` | `MOV dest, x` | Subtractive identity |
| `x - x` | `MOV dest, 0` | Self-subtraction |
| `x * 1` or `1 * x` | `MOV dest, x` | Multiplicative identity |
| `x * 0` or `0 * x` | `MOV dest, 0` | Multiplicative annihilator |
| `x / 1` | `MOV dest, x` | Division identity |
| `x / x` ($x \neq 0$) | `MOV dest, 1` | Self-division |
| `x % 1` | `MOV dest, 0` | Modulo identity |

#### Transformation Example
```text
# Before Algebraic Simplification
t1 = c + 0
t2 = t1 * 1
t3 = d * 0

# After Algebraic Simplification
t1 = c
t2 = t1
t3 = 0
```

---

### 2.4 Common Subexpression Elimination (`Optimizer.cpp`)

#### Algorithm
Detects duplicate expressions within basic blocks using a value-numbering cache:
$$\text{exprTable} : (\text{OpCode}, \text{Operand}_1, \text{Operand}_2) \to \text{Result Variable}$$

- **Commutativity Normalization**: For commutative operators (`+`, `*`, `==`, `!=`), operands are sorted lexicographically before hashing. Thus, `x + y` and `y + x` produce the identical key `(ADD, "x", "y")`.
- When an expression has already been computed, the instruction is replaced with a copy assignment (`MOV dest, existingVar`).
- **Dependency Invalidation**: If an instruction modifies variable $V$, all cached expressions referencing $V$ as an operand are purged from $\text{exprTable}$.
- **Control Flow Flush**: Cleared at block boundaries.

#### Transformation Example
```text
# Before CSE
t1 = x + y
a = t1
t2 = y + x
b = t2

# After CSE
t1 = x + y
a = t1
t2 = t1       # Reuses t1 without recomputing x + y
b = t2
```

---

### 2.5 Dead Code Elimination (`DeadCodeElimination.h`, `DeadCodeElimination.cpp`)

#### Algorithm
Performs **Backward Liveness Analysis**:
1. **Root Identification**:
   - The final program output variable and variables read anywhere in the code are initially marked live.
   - Operands of branch conditions (`if_false cond goto Label`) are marked live.
   - Control-flow instructions (`LABEL`, `JUMP`, `JUMP_IF_FALSE`) are unconditionally preserved.
2. **Backward Sweep**:
   - Traverses instructions from bottom to top.
   - If an instruction defines `dest`:
     - If `dest` is currently in the live set:
       - Keep the instruction.
       - Remove `dest` from the live set (the definition kills prior liveness, eliminating dead stores).
       - Add operands `src1` and `src2` to the live set (generating operand liveness).
     - If `dest` is NOT in the live set:
       - Eliminate the instruction!
3. Eliminates redundant self-moves (`dest = dest`).

#### Transformation Example
```text
# Before Dead Code Elimination
a = 10
unused1 = 50
unused2 = unused1 * 2
t1 = a + 5
b = t1

# After Dead Code Elimination
a = 10
t1 = a + 5
b = t1
```
*(Both `unused1` and `unused2` are completely removed).*

---

### 2.6 Control Flow Optimization (`Optimizer.cpp`)

#### Algorithm
1. **Constant Branch Folding**:
   - `if_false 1 goto L`: Condition is always true; branch is never taken. The instruction is removed (fall-through).
   - `if_false 0 goto L`: Condition is always false; branch is always taken. Converted to unconditional `goto L`.
2. **Unreachable Code Elimination**:
   - Any instructions following an unconditional `goto L` up to the next `LABEL` are unreachable dead code and are discarded.
3. **Redundant Jump Elimination**:
   - If `goto L` is immediately followed by target `L:`, the jump is redundant and removed.

#### Transformation Example
```text
# Before Control Flow Optimization
if_false 0 goto L2
t1 = x + 1
y = t1
goto L2
L2:

# After Control Flow Optimization
goto L2
L2:

# After Redundant Jump Elimination
L2:
```
