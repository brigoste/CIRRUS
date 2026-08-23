# Equation System Architecture

## Overview

CIRRUS separates the **mathematical concept of an equation system** from the **storage and implementation details of a particular linear algebra representation**.

The equation-system hierarchy is designed to provide a stable interface between:

* finite-volume discretization and assembly,
* linear and nonlinear solvers,
* matrix/vector storage,
* preconditioners,
* and future matrix-free or block-based formulations.

The current hierarchy is:

```text
                         EquationSystem
                               │
                               │
                               ▼
                    LinearEquationSystem
                               │
                               │
                               ▼
                         LinearSystem
```

The important distinction is that each layer describes a progressively more specific concept.

```text
EquationSystem
    │
    │  "I represent equations."
    │
    ▼
LinearEquationSystem
    │
    │  "Those equations are linear:
    │   A x = b"
    │
    ▼
LinearSystem
       "I store that linear system using
        an explicit sparse matrix representation."
```

This separation allows solvers to depend only on the capabilities they actually require.

---

# 1. Design Goals

The equation-system abstraction exists to prevent the rest of CIRRUS from becoming coupled to one particular matrix implementation.

For example, the current `LinearSystem` stores the matrix as sparse rows:

```cpp
std::vector<std::vector<std::pair<std::size_t, double>>> A_;
```

That is an implementation detail.

A solver such as Conjugate Gradient does not fundamentally care how the matrix is stored. It only needs to perform operations such as:

```text
A x
```

and access the right-hand side.

Similarly, a future matrix-free system may not store `A` at all.

The architecture therefore separates:

```text
mathematical abstraction
        ↓
solver requirements
        ↓
storage implementation
```

rather than making the solver depend directly on storage.

---

# 2. Inheritance Structure

The primary inheritance relationship is:

```text
┌───────────────────────────────┐
│        EquationSystem         │
│                               │
│  General equation system      │
│  interface                    │
└───────────────┬───────────────┘
                │
                │ inherits
                ▼
┌───────────────────────────────┐
│     LinearEquationSystem      │
│                               │
│  Linear system interface      │
│                               │
│        A x = b                │
└───────────────┬───────────────┘
                │
                │ inherits
                ▼
┌───────────────────────────────┐
│         LinearSystem          │
│                               │
│  Explicit sparse matrix       │
│  implementation               │
│                               │
│  A, b storage                 │
└───────────────────────────────┘
```

The first two classes are **interfaces**.

`LinearSystem` is the first concrete implementation.

---

# 3. `EquationSystem`

## Responsibility

`EquationSystem` represents the most general concept:

> A collection of equations that a numerical method can operate on.

It deliberately does **not** assume that the equations are linear.

The current interface is:

```cpp
class EquationSystem
{
public:
    virtual ~EquationSystem() = default;

    virtual std::size_t size() const = 0;

    virtual void addCoeff(
        std::size_t i,
        std::size_t j,
        double value) = 0;

    virtual void addRHS(
        std::size_t i,
        double value) = 0;

    virtual void clear() = 0;
};
```

## What it provides

### `size()`

Returns the number of equations/unknowns represented by the system.

Conceptually:

```text
number of equations
        =
number of unknowns
```

for the square systems currently used by CIRRUS.

### `addCoeff(i, j, value)`

Adds a contribution to the coefficient associated with equation `i` and unknown `j`.

For a linear system this corresponds to:

```text
A[i,j] += value
```

The important word is **adds**.

Finite-volume assembly naturally accumulates contributions:

```text
face contribution
        ↓
cell equation
        ↓
matrix coefficient
```

Therefore the equation-system interface exposes accumulation semantics rather than requiring the assembler to know how the matrix is stored.

### `addRHS(i, value)`

Adds a contribution to equation `i`'s right-hand side:

```text
b[i] += value
```

### `clear()`

Resets the assembled equation system so that it can be reused.

---

# 4. What `EquationSystem` Does NOT Know

The general `EquationSystem` should not know about:

* sparse matrices,
* CSR/CSC storage,
* diagonal storage,
* rows,
* matrix-vector multiplication,
* preconditioners,
* iterative solvers,
* TDMA,
* CG,
* BiCGSTAB,
* or any particular matrix representation.

That is intentional.

The general interface describes **what an equation system is capable of doing**, not how it stores its data.

---

# 5. `LinearEquationSystem`

`LinearEquationSystem` specializes `EquationSystem` for linear systems.

Its mathematical model is:

```text
A x = b
```

The current interface adds:

```cpp
class LinearEquationSystem : public EquationSystem
{
public:
    virtual ~LinearEquationSystem() = default;

    virtual double coeff(
        std::size_t i,
        std::size_t j) const = 0;

    virtual double rhs(
        std::size_t i) const = 0;

    virtual void matvec(
        const std::vector<double>& x,
        std::vector<double>& y) const = 0;
};
```

This is the layer where the mathematical structure becomes explicitly linear.

---

# 6. Why `LinearEquationSystem` Exists

This intermediary is important because not every equation system is necessarily linear.

The distinction is:

```text
EquationSystem
│
├── LinearEquationSystem
│      │
│      └── LinearSystem
│
└── Future nonlinear system
       │
       └── NonlinearEquationSystem
```

For example, a future nonlinear solver could operate on:

```text
F(x) = 0
```

rather than:

```text
A x = b
```

A nonlinear system does not necessarily have a meaningful fixed matrix `A`.

Therefore it should not be forced to implement linear-specific operations such as:

```cpp
coeff(i, j)
matvec(x, y)
rhs(i)
```

The hierarchy allows us to distinguish those cases cleanly.

---

# 7. Responsibilities of `LinearEquationSystem`

`LinearEquationSystem` defines the operations that a solver can reasonably expect from **any linear equation system**, regardless of storage implementation.

## `coeff(i,j)`

Provides access to a coefficient:

```text
A[i,j]
```

The solver can ask for a matrix coefficient without knowing where or how it is stored.

## `rhs(i)`

Provides:

```text
b[i]
```

again without exposing storage.

## `matvec(x,y)`

Computes:

```text
y = A x
```

This is particularly important.

A solver should not need to do:

```cpp
for (...)
{
    for (...)
    {
        ...
    }
}
```

over the matrix's internal storage.

Instead it can simply request:

```cpp
sys.matvec(x, y);
```

This is what makes the interface compatible with future matrix-free implementations.

---

# 8. `LinearSystem`

`LinearSystem` is the current concrete implementation of `LinearEquationSystem`.

It represents an explicitly stored sparse matrix.

Conceptually:

```text
LinearSystem

A:
┌───────┬───────┬───────┐
│ A00   │ A01   │       │
│ A10   │ A11   │ A12   │
│       │ A21   │ A22   │
└───────┴───────┴───────┘

b:
┌─────┐
│ b0  │
│ b1  │
│ b2  │
└─────┘
```

The current implementation uses sparse row storage.

Conceptually:

```cpp
A_[i] = {
    {column, value},
    {column, value},
    ...
};
```

Therefore:

```text
LinearSystem
    │
    ├── A_
    │    └── sparse row storage
    │
    ├── b_
    │    └── RHS vector
    │
    └── n_
         └── system size
```

These are **implementation-specific variables**.

---

# 9. `LinearSystem` Responsibilities

`LinearSystem` is responsible for translating the abstract linear-system interface into its actual storage representation.

For example:

```cpp
void LinearSystem::addCoeff(
    std::size_t i,
    std::size_t j,
    double value)
```

must know how to find the appropriate sparse entry and accumulate the value.

Likewise:

```cpp
double LinearSystem::coeff(
    std::size_t i,
    std::size_t j) const
```

searches the sparse row.

And:

```cpp
void LinearSystem::matvec(
    const std::vector<double>& x,
    std::vector<double>& y) const
```

performs the matrix-vector multiplication using the sparse representation.

The solver does not need to know any of this.

---

# 10. Responsibility by Layer

A useful way to remember the hierarchy is:

| Class                  | Responsibility                                           |
| ---------------------- | -------------------------------------------------------- |
| `EquationSystem`       | Defines the general equation-system interface            |
| `LinearEquationSystem` | Defines the interface required by linear solvers         |
| `LinearSystem`         | Implements a linear system using explicit sparse storage |

Or visually:

```text
┌─────────────────────────────────────────────┐
│ EquationSystem                              │
│                                             │
│ General equation assembly                   │
│ size()                                      │
│ addCoeff()                                  │
│ addRHS()                                    │
│ clear()                                     │
└──────────────────────┬──────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────┐
│ LinearEquationSystem                        │
│                                             │
│ Linear-system operations                    │
│ coeff()                                     │
│ rhs()                                       │
│ matvec()                                    │
└──────────────────────┬──────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────┐
│ LinearSystem                                │
│                                             │
│ Concrete implementation                     │
│                                             │
│ A_  → sparse matrix storage                 │
│ b_  → RHS vector                            │
│ n_  → system size                           │
└─────────────────────────────────────────────┘
```

---

# 11. Where the Data Lives

One of the most important architectural distinctions is which layer owns which data.

## `EquationSystem`

Should contain only data required by the **general equation-system concept**, if any.

Currently:

```text
No required storage
```

The interface itself does not need to own a matrix or vector.

---

## `LinearEquationSystem`

Also ideally contains no concrete matrix storage.

It describes the operations available for:

```text
A x = b
```

but does not dictate how `A` or `b` are stored.

Conceptually:

```text
LinearEquationSystem
        │
        ├── coeff()
        ├── rhs()
        └── matvec()
```

These are capabilities, not storage.

---

## `LinearSystem`

This is where the concrete storage belongs.

Current implementation:

```text
n_
A_
b_
```

where:

```text
n_ = number of equations

A_ = sparse coefficient matrix

b_ = RHS vector
```

Additional implementation-specific data can also live here if required by the chosen storage strategy.

---

# 12. Assembly vs Solution

The architecture intentionally separates **assembly** from **solution**.

The finite-volume side does something like:

```text
Mesh
 │
 ▼
Discretization
 │
 ▼
Operator
 │
 ▼
EquationSystem
 │
 ├── addCoeff()
 └── addRHS()
```

The solver side does:

```text
EquationSystem
 │
 ▼
LinearEquationSystem
 │
 ▼
Linear Solver
 │
 ▼
solution x
```

This creates a clean boundary:

```text
                ASSEMBLY
                   │
                   ▼
        ┌────────────────────┐
        │  Equation System   │
        └─────────┬──────────┘
                  │
                  │
            SOLUTION PHASE
                  │
                  ▼
             Linear Solver
```

The assembler should not care which solver will eventually consume the system.

Likewise, the solver should not care which finite-volume operator assembled it.

---

# 13. Example: Finite-Volume Assembly

Suppose a cell equation produces:

```text
a_P φ_P + a_E φ_E + a_W φ_W = b
```

The discretization layer might produce:

```cpp
system.addCoeff(P, P, aP);
system.addCoeff(P, E, aE);
system.addCoeff(P, W, aW);

system.addRHS(P, b);
```

The assembler only sees:

```cpp
EquationSystem&
```

It does not need to know whether the implementation is:

```text
LinearSystem
CSRMatrixSystem
BlockLinearSystem
MatrixFreeLinearSystem
...
```

That is the key benefit.

---

# 14. Example: CG

CG requires a linear system.

Therefore its interface should be:

```cpp
std::vector<double> CG(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    const Preconditioner& M);
```

CG can then do:

```cpp
std::vector<double> Ap(N);

sys.matvec(p, Ap);
```

It does not need:

```cpp
sys.row(i)
```

or:

```cpp
sys.A_
```

or any knowledge of the sparse matrix representation.

The solver sees only the abstraction it needs.

---

# 15. Example: BiCGSTAB

BiCGSTAB has the same requirement.

It operates on:

```text
A x = b
```

and therefore accepts:

```cpp
const LinearEquationSystem&
```

It can perform:

```cpp
sys.matvec(p_hat, v);
sys.matvec(s_hat, t);
```

without knowing how `A` is represented.

---

# 16. Example: TDMA

TDMA is slightly different.

The Thomas algorithm requires a very specific matrix structure:

```text
┌─────┬─────┐
│ b₀  │ c₀  │
├─────┼─────┼─────┐
│ a₁  │ b₁  │ c₁  │
├─────┼─────┼─────┤
│     │ a₂  │ b₂  │ c₂
└─────┴─────┴─────┘
```

Therefore TDMA needs access to the tridiagonal coefficients.

This raises an important architectural question for the future:

> Should TDMA remain dependent on the concrete `LinearSystem`, or should a more specific interface such as `TridiagonalLinearSystem` be introduced?

A clean future hierarchy could be:

```text
EquationSystem
       │
       ▼
LinearEquationSystem
       │
       ├───────────────┐
       ▼               ▼
LinearSystem     TridiagonalEquationSystem
                       │
                       ▼
                     TDMA
```

We do **not** need to introduce that abstraction prematurely.

The current architecture should first establish the general linear interface.

---

# 17. Preconditioners

Preconditioners also operate on linear systems.

The current interface is:

```cpp
virtual void setup(
    const LinearEquationSystem& sys) = 0;

virtual void apply(
    const std::vector<double>& r,
    std::vector<double>& z) const = 0;
```

This is exactly the type of separation the hierarchy is intended to provide.

For example:

```text
                 LinearEquationSystem
                         │
             ┌───────────┴───────────┐
             │                       │
             ▼                       ▼
         CG / BiCGSTAB        Preconditioner
                                     │
                         ┌───────────┼───────────┐
                         ▼           ▼           ▼
                      None        Jacobi        ILU0
```

Jacobi and ILU0 may require different implementation-specific access in the future, but the public solver/preconditioner contract remains based on the linear-system abstraction.

---

# 18. Future Matrix-Free Implementation

This is one of the major reasons for introducing `LinearEquationSystem`.

Consider a future implementation:

```cpp
class MatrixFreeLinearSystem : public LinearEquationSystem
{
public:

    double coeff(...) const override;

    double rhs(...) const override;

    void matvec(
        const std::vector<double>& x,
        std::vector<double>& y) const override;
};
```

It might not store:

```cpp
A_
```

at all.

Instead:

```text
x
│
▼
matrix-free operator
│
▼
y = A(x)
```

The CG solver would not change.

It would still call:

```cpp
sys.matvec(x, y);
```

This is exactly what we mean by **decoupling the solver from matrix storage**.

---

# 19. Future Block Systems

The same idea applies to coupled CFD equations.

Eventually CIRRUS may need systems such as:

```text
[ A_uu  A_uv ] [u]   [b_u]
[ A_vu  A_vv ] [v] = [b_v]
```

or larger systems involving:

```text
u
v
w
p
T
```

A future abstraction could introduce a block linear system:

```text
EquationSystem
       │
       ▼
LinearEquationSystem
       │
       ├───────────────┐
       │               │
       ▼               ▼
LinearSystem      BlockLinearSystem
```

Again, CG/BiCGSTAB or future block solvers can be written against the appropriate abstraction rather than directly against one storage implementation.

---

# 20. Future Nonlinear Systems

The top-level abstraction also leaves room for nonlinear solution strategies.

A nonlinear equation system may be represented conceptually as:

```text
F(x) = 0
```

rather than:

```text
A x = b
```

The hierarchy could eventually become:

```text
                         EquationSystem
                               │
                ┌──────────────┴──────────────┐
                │                             │
                ▼                             ▼
      LinearEquationSystem          NonlinearEquationSystem
                │                             │
        ┌───────┴───────┐                     │
        │               │                     │
        ▼               ▼                     ▼
   LinearSystem    BlockLinearSystem    Newton / Picard
```

This is a major reason not to make `EquationSystem` itself a "matrix interface."

It is intended to represent the broader mathematical concept.

---

# 21. What We Should Avoid

The abstraction becomes less useful if we start leaking implementation details upward.

For example, this would be undesirable:

```cpp
EquationSystem& system;

system.row(i);
system.nnz();
system.diagonal(i);
system.A_;
```

Those are characteristics of one particular sparse implementation.

Instead, generic consumers should use:

```cpp
system.size();
system.addCoeff(...);
system.addRHS(...);
system.clear();
```

and linear consumers should use:

```cpp
system.coeff(...);
system.rhs(...);
system.matvec(...);
```

The rule is:

> Depend on the narrowest interface that provides everything the algorithm needs.

---

# 22. Current CIRRUS Architecture

The equation-system layer currently fits into the broader CFD architecture as follows:

```text
                    ┌──────────────┐
                    │     Mesh     │
                    └──────┬───────┘
                           │
                           ▼
                    ┌──────────────┐
                    │ Discretizer  │
                    └──────┬───────┘
                           │
                           ▼
                    ┌──────────────┐
                    │   Operator   │
                    └──────┬───────┘
                           │
                           │ assembly
                           ▼
                 ┌─────────────────────┐
                 │   EquationSystem    │
                 └──────────┬──────────┘
                            │
                            ▼
                 ┌─────────────────────┐
                 │ LinearEquationSystem│
                 └──────────┬──────────┘
                            │
              ┌─────────────┼──────────────┐
              │             │              │
              ▼             ▼              ▼
            TDMA           CG          BiCGSTAB
              │             │              │
              └─────────────┼──────────────┘
                            ▼
                       solution x
```

The concrete storage sits underneath the abstraction:

```text
                 LinearEquationSystem
                          ▲
                          │
                   implements
                          │
                   LinearSystem
                          │
                 ┌────────┴────────┐
                 │                 │
                 ▼                 ▼
                A_                b_
          sparse matrix         RHS vector
```

---

# 23. The Most Important Mental Model

The easiest way to understand the architecture is to think of the three classes as three questions.

### `EquationSystem`

> **What is the general interface for assembling equations?**

It provides the assembly contract.

---

### `LinearEquationSystem`

> **What additional operations are required when those equations form A x = b?**

It provides the linear-algebra contract.

---

### `LinearSystem`

> **How do we actually store and execute that linear system right now?**

It provides the concrete sparse implementation.

---

# 24. Why This Is Worth Doing Now

The current CIRRUS solver could technically operate directly on `LinearSystem`.

The problem would appear later.

Without this abstraction:

```text
Finite-volume assembly
        │
        ▼
   LinearSystem
        │
        ├── CG
        ├── BiCGSTAB
        ├── SOR
        ├── Jacobi
        ├── ILU0
        └── TDMA
```

Everything becomes coupled to the current sparse representation.

With the abstraction:

```text
Finite-volume assembly
        │
        ▼
  EquationSystem
        │
        ▼
LinearEquationSystem
        │
        ├── CG
        ├── BiCGSTAB
        ├── future solvers
        │
        ├───────────────┐
        ▼               ▼
 LinearSystem     future implementations
        │
        ├── sparse
        ├── matrix-free
        ├── block
        └── specialized
```

That gives CIRRUS room to evolve without rewriting the numerical algorithms every time the underlying linear-algebra representation changes.

---

# 25. Current Status

The equation-system abstraction currently provides:

* [x] General `EquationSystem` interface
* [x] `LinearEquationSystem` specialization
* [x] Concrete sparse `LinearSystem`
* [x] Generic assembly through `EquationSystem&`
* [x] Linear solvers operating on `LinearEquationSystem`
* [x] Matrix-vector multiplication exposed through the linear interface
* [x] Preconditioners operating against the linear abstraction
* [x] Existing verification suite passing after the abstraction refactor

The immediate architectural direction is therefore:

```text
EquationSystem
       │
       ▼
LinearEquationSystem
       │
       ▼
LinearSystem
       │
       ├── sparse explicit matrix
       │
       └── current production implementation
```

Future extensions should be added **only when a real numerical requirement appears**, rather than creating speculative interfaces for every possible matrix format.

The next planned layer is the **matrix/vector assembly interface**, followed eventually by matrix-free operator support and other specialized linear-algebra implementations.
