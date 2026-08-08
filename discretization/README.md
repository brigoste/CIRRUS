# Discretization Framework

The `discretization/` module contains the finite-volume discretization framework used by CIRRUS.

Its primary responsibility is to transform physical-model information, mesh geometry, and boundary conditions into discrete flux contributions and ultimately into a linear system suitable for solution.

The framework is deliberately divided into two major stages:

1. **Flux construction** — determine the physical/discretization contributions at faces and cells.
2. **Operator assembly** — convert those contributions into matrix and right-hand-side terms.

This separation allows the discretization logic to remain independent of the details of the linear-system implementation.

---

## Directory Structure

```text
discretization/
│
├── builders/
│   ├── ConvectionFluxBuilder.cpp
│   ├── ConvectionFluxBuilder.hpp
│   ├── DiffusionFluxBuilder.cpp
│   ├── DiffusionFluxBuilder.hpp
│   ├── SourceFluxBuilder.cpp
│   └── SourceFluxBuilder.hpp
│
├── convection/
│   ├── CentralDifferenceScheme.cpp
│   ├── CentralDifferenceScheme.hpp
│   ├── ConvectionScheme.hpp
│   ├── UpwindScheme.cpp
│   └── UpwindScheme.hpp
│
├── diffusion/
│   ├── DiffusionOperator.cpp
│   └── DiffusionOperator.hpp
│
├── gradient/
│   ├── GradientScheme.hpp
│   ├── GradientType.hpp
│   ├── GreenGaussGradient.cpp
│   ├── GreenGaussGradient.hpp
│   ├── LeastSquaresGradient.cpp
│   └── LeastSquaresGradient.hpp
│
├── operators/
│   ├── ConvectionOperator.cpp
│   ├── ConvectionOperator.hpp
│   └── Operator.hpp
│
├── CellResidual.hpp
├── FaceConvection.hpp
├── FaceDiffusion.hpp
├── FaceType.hpp
├── FiniteVolumeOperator.cpp
├── FiniteVolumeOperator.hpp
├── FluxAccumulator.hpp
├── FluxBuilder.cpp
└── FluxBuilder.hpp
```

---

# Architecture

The overall discretization pipeline is:

```text
                         ┌─────────────────────┐
                         │      Simulation     │
                         └──────────┬──────────┘
                                    │
                                    ▼
                         ┌─────────────────────┐
                         │    FluxBuilder      │
                         │    (orchestrator)   │
                         └──────────┬──────────┘
                                    │
              ┌─────────────────────┼─────────────────────┐
              │                     │                     │
              ▼                     ▼                     ▼
     ┌────────────────┐    ┌────────────────┐    ┌────────────────┐
     │DiffusionFlux   │    │ConvectionFlux  │    │SourceFlux      │
     │Builder         │    │Builder         │    │Builder         │
     └───────┬────────┘    └───────┬────────┘    └───────┬────────┘
             │                     │                     │
             └─────────────────────┼─────────────────────┘
                                   ▼
                         ┌─────────────────────┐
                         │  FluxAccumulator    │
                         │                     │
                         │ • diffusion fluxes  │
                         │ • convection fluxes │
                         │ • cell Su / Sp      │
                         └──────────┬──────────┘
                                    │
                                    ▼
                       ┌────────────────────────┐
                       │ FiniteVolumeOperator   │
                       │      (orchestrator)    │
                       └────────────┬───────────┘
                                    │
                    ┌───────────────┴───────────────┐
                    │                               │
                    ▼                               ▼
          ┌─────────────────┐             ┌──────────────────┐
          │DiffusionOperator│             │ConvectionOperator│
          └────────┬────────┘             └────────┬─────────┘
                   │                               │
                   │                               ▼
                   │                     ┌─────────────────┐
                   │                     │ConvectionScheme │
                   │                     └────────┬────────┘
                   │                              │
                   ▼                              │
          ┌─────────────────┐                     │
          │ Diffusion       │                     │
          │ assembly        │                     │
          └────────┬────────┘                     │
                   │                              │
                   └──────────────┬───────────────┘
                                  ▼
                         ┌─────────────────────┐
                         │    LinearSystem     │
                         │                     │
                         │    Matrix + RHS     │
                         └─────────────────────┘
```

The important architectural boundary is:

```text
             FLUX CONSTRUCTION
                    │
                    ▼
             FluxAccumulator
                    │
                    ▼
             OPERATOR ASSEMBLY
                    │
                    ▼
              LinearSystem
```

This prevents the flux builders from needing to know how the linear system is represented.

---

# 1. Flux Construction

## `FluxBuilder`

**Files:**

```text
FluxBuilder.cpp
FluxBuilder.hpp
```

`FluxBuilder` is the top-level coordinator for flux construction.

It does not perform the individual discretization calculations itself. Instead, it delegates to the specialized builders:

```text
FluxBuilder
    │
    ├── DiffusionFluxBuilder
    ├── ConvectionFluxBuilder
    └── SourceFluxBuilder
```

The current construction sequence is:

```cpp
diffusionFlux_.apply(...);
convectionFlux_.apply(...);
sourceFlux_.apply(...);
```

All three builders write their contributions into the same `FluxAccumulator`.

This provides a single entry point for constructing the complete set of discretization contributions for a simulation.

---

## `builders/`

The builder classes are responsible for translating mesh, physics, and boundary-condition information into discrete flux contributions.

They do **not** assemble the linear system.

### `DiffusionFluxBuilder`

```text
builders/DiffusionFluxBuilder.cpp
builders/DiffusionFluxBuilder.hpp
```

Constructs diffusion-related flux contributions.

Responsibilities include:

* Iterating over interior faces.
* Computing face diffusion coefficients.
* Adding interior diffusion couplings.
* Processing Dirichlet boundary conditions.
* Processing Neumann boundary conditions.
* Processing convective/Robin boundary conditions.
* Handling manufactured verification boundary conditions.

Interior diffusion is represented as a coupling:

```text
P <──── D ────> N
```

Boundary contributions are placed into the cell residual through `FluxAccumulator`.

---

### `ConvectionFluxBuilder`

```text
builders/ConvectionFluxBuilder.cpp
builders/ConvectionFluxBuilder.hpp
```

Constructs convection flux information for interior faces.

For each interior face it obtains the face mass/convective flux from the physics model and stores:

```text
(P, N, F)
```

in the `FluxAccumulator`.

The builder does not determine how that flux becomes matrix coefficients. That responsibility belongs to the selected `ConvectionScheme`.

This separation allows schemes such as upwind and central differencing to operate on the same face-flux data.

---

### `SourceFluxBuilder`

```text
builders/SourceFluxBuilder.cpp
builders/SourceFluxBuilder.hpp
```

Constructs cell-centered source contributions.

For each cell it:

1. Obtains the physical source from the `PhysicsModel`.
2. Multiplies the source by the cell volume.
3. Adds the resulting contribution to `Su`.
4. Adds manufactured verification forcing when a `VerificationCase` is active.

The builder therefore handles both physical sources and verification-specific forcing without coupling the rest of the discretization framework to the verification system.

---

# 2. Flux Storage

## `FluxAccumulator`

```text
FluxAccumulator.hpp
```

`FluxAccumulator` is the intermediate data structure between flux construction and linear-system assembly.

It stores three categories of information:

```text
FluxAccumulator
│
├── CellResidual
│   ├── Su
│   └── Sp
│
├── FaceDiffusion
│   └── diffusion contributions
│
└── FaceConvection
    └── convection contributions
```

The accumulator is reset and reused for each assembly.

```cpp
flux.reset();
```

This avoids repeatedly reconstructing the entire data structure during simulation iterations.

### Copy semantics

`FluxAccumulator` is intentionally non-copyable.

It supports move construction and move assignment but does not allow accidental copying of potentially large flux data.

---

# 3. Flux Data Structures

## `CellResidual`

```text
CellResidual.hpp
```

Stores cell-centered source information:

```cpp
struct CellResidual
{
    double Su = 0.0;
    double Sp = 0.0;
};
```

Where:

* `Su` is the explicit source/RHS contribution.
* `Sp` is the linearized implicit source contribution.

The final matrix assembly converts these values into linear-system terms.

---

## `FaceDiffusion`

```text
FaceDiffusion.hpp
```

Represents a diffusion coupling associated with a face.

It contains:

```text
P
N
D
type
```

where:

* `P` is the owner cell.
* `N` is the neighbor cell.
* `D` is the diffusion coefficient/conductance.
* `type` identifies whether the contribution is interior or boundary.

---

## `FaceConvection`

```text
FaceConvection.hpp
```

Represents a convection flux between two cells:

```text
P
N
F
```

where:

* `P` is the owner cell.
* `N` is the neighboring cell.
* `F` is the face convective flux.

The actual discretization of this flux is delegated to a convection scheme.

---

## `FaceType`

```text
FaceType.hpp
```

Identifies the type of face contribution:

```cpp
enum class FaceType
{
    Interior,
    Boundary
};
```

This is primarily used when interpreting stored face contributions during assembly.

---

# 4. Operators

The operator layer converts the accumulated flux information into linear-system contributions.

The fundamental abstraction is `Operator`.

## `Operator`

```text
operators/Operator.hpp
```

Defines the common operator interface:

```cpp
virtual void assemble(
    const FluxAccumulator& flux,
    LinearSystem& sys
) const = 0;
```

An operator therefore consumes:

```text
FluxAccumulator
```

and produces:

```text
LinearSystem
```

This establishes a generic interface for future discretization operators.

---

## `DiffusionOperator`

```text
diffusion/DiffusionOperator.cpp
diffusion/DiffusionOperator.hpp
```

The `DiffusionOperator` performs matrix assembly for diffusion contributions.

For an interior face coupling between cells `P` and `N`, the operator adds:

```text
       P       N
P     +D      -D
N     -D      +D
```

This produces the standard symmetric diffusion stencil.

The actual construction of `D` occurs earlier in `DiffusionFluxBuilder`.

This is an intentional separation:

```text
DiffusionFluxBuilder
    │
    │ determines D
    ▼
FluxAccumulator
    │
    │ stores D
    ▼
DiffusionOperator
    │
    │ assembles matrix coefficients
    ▼
LinearSystem
```

---

## `ConvectionOperator`

```text
operators/ConvectionOperator.cpp
operators/ConvectionOperator.hpp
```

The `ConvectionOperator` assembles stored convection fluxes.

It delegates the actual coefficient calculation to the configured `ConvectionScheme`:

```cpp
for (const auto& f : flux.convection())
{
    scheme_.assemble(f, sys);
}
```

This means the operator does not need to know whether the simulation uses:

* Upwind differencing.
* Central differencing.
* A future higher-order scheme.

The scheme controls the discretization while the operator controls orchestration.

---

## `FiniteVolumeOperator`

```text
FiniteVolumeOperator.cpp
FiniteVolumeOperator.hpp
```

`FiniteVolumeOperator` is the high-level assembly coordinator.

Its current assembly sequence is:

```text
1. Diffusion
2. Convection
3. Sources and boundary contributions
```

Conceptually:

```text
FluxAccumulator
      │
      ├── diffusion ──────► DiffusionOperator
      │
      ├── convection ─────► ConvectionOperator
      │
      └── Su / Sp ─────────► LinearSystem
```

For cell-centered source contributions:

```cpp
sys.addRHS(c, flux[c].Su);
sys.addCoeff(c, c, -flux[c].Sp);
```

Thus `FiniteVolumeOperator` provides the common finite-volume assembly pathway without requiring the individual operators to know about one another.

---

# 5. Convection Schemes

The convection subsystem defines how face convection fluxes are converted into discrete coefficients.

```text
convection/
├── ConvectionScheme.hpp
├── CentralDifferenceScheme.cpp/.hpp
└── UpwindScheme.cpp/.hpp
```

## `ConvectionScheme`

Defines the common interface for convection discretization.

The `ConvectionOperator` depends only on this abstraction.

Therefore:

```text
ConvectionOperator
        │
        ▼
ConvectionScheme
     /       \
    ▼         ▼
 Upwind    Central
```

New convection schemes can be added without modifying `ConvectionOperator`.

---

## `UpwindScheme`

Implements first-order upwind convection discretization.

The direction of the face flux determines which cell contributes to the resulting coefficient.

---

## `CentralDifferenceScheme`

Implements central-difference convection discretization.

The scheme distributes the face contribution according to the central-difference formulation rather than selecting a single upstream cell.

---

# 6. Gradient Schemes

The gradient subsystem is currently separate from the primary flux/assembly path.

```text
gradient/
├── GradientScheme.hpp
├── GradientType.hpp
├── GreenGaussGradient.cpp/.hpp
└── LeastSquaresGradient.cpp/.hpp
```

## `GradientScheme`

Defines the common interface for gradient reconstruction.

Possible implementations include:

```text
GradientScheme
    │
    ├── GreenGaussGradient
    └── LeastSquaresGradient
```

Gradient reconstruction is intended to provide cell gradients for discretizations that require them, such as higher-order schemes or non-orthogonal corrections.

### Current status

Gradient reconstruction is **not currently part of the active finite-volume assembly pipeline**.

There is intentionally no `GradientOperator` abstraction in the current architecture. Gradient schemes should be integrated at the point where a discretization actually requires gradient information rather than being owned unconditionally by `Simulation`.

This keeps the current solve path free of unused dependencies.

---

# 7. Current Assembly Sequence

A simulation assembly proceeds conceptually as follows.

### Step 1 — Reset

The `FluxAccumulator` is cleared:

```text
FluxAccumulator.reset()
```

This removes contributions from the previous assembly.

---

### Step 2 — Construct fluxes

`FluxBuilder` invokes the specialized builders:

```text
FluxBuilder
    │
    ├── DiffusionFluxBuilder
    ├── ConvectionFluxBuilder
    └── SourceFluxBuilder
```

The builders populate:

```text
FluxAccumulator
```

---

### Step 3 — Assemble diffusion

`DiffusionOperator` reads the diffusion contributions and adds the corresponding matrix coefficients.

For an interior connection:

```text
        D
P ───────────── N

A[P,P] += D
A[P,N] -= D
A[N,N] += D
A[N,P] -= D
```

---

### Step 4 — Assemble convection

`ConvectionOperator` reads the stored convection fluxes and passes them to the selected `ConvectionScheme`.

```text
FaceConvection
      │
      ▼
ConvectionScheme
      │
      ▼
LinearSystem
```

---

### Step 5 — Assemble sources and implicit terms

Cell residual data is converted into matrix and RHS contributions:

```text
Su → RHS
Sp → diagonal coefficient
```

Specifically:

```cpp
sys.addRHS(c, flux[c].Su);
sys.addCoeff(c, c, -flux[c].Sp);
```

---

### Step 6 — Solve

After assembly, the resulting `LinearSystem` is passed to the selected linear solver.

The discretization layer itself does not solve the system.

---

# 8. Separation of Responsibilities

The current architecture intentionally separates several concerns.

| Component               | Responsibility                             |
| ----------------------- | ------------------------------------------ |
| `FluxBuilder`           | Coordinates flux construction              |
| `DiffusionFluxBuilder`  | Constructs diffusion flux data             |
| `ConvectionFluxBuilder` | Constructs convection flux data            |
| `SourceFluxBuilder`     | Constructs source data                     |
| `FluxAccumulator`       | Stores intermediate discretization data    |
| `DiffusionOperator`     | Assembles diffusion matrix contributions   |
| `ConvectionOperator`    | Coordinates convection matrix assembly     |
| `ConvectionScheme`      | Determines convection coefficients         |
| `FiniteVolumeOperator`  | Coordinates complete FV matrix assembly    |
| `GradientScheme`        | Computes cell gradients when required      |
| `LinearSystem`          | Stores the final discrete algebraic system |

The key principle is:

> **Builders determine what the discretization contributes; operators determine how those contributions enter the algebraic system.**

---

# 9. Extension Points

The current structure is intended to support future extensions without requiring major changes to the existing assembly framework.

Potential extensions include:

### Additional convection schemes

```text
convection/
├── ...
├── QUICKScheme
├── MUSCLScheme
└── HigherOrderScheme
```

These can implement `ConvectionScheme` without changing `ConvectionOperator`.

### Additional gradient schemes

```text
gradient/
├── ...
├── WeightedLeastSquaresGradient
└── ...
```

These can implement `GradientScheme`.

### Additional physical fluxes

The builder pattern can be extended with additional specialized builders if the governing equations require them.

For example:

```text
FluxBuilder
    │
    ├── DiffusionFluxBuilder
    ├── ConvectionFluxBuilder
    ├── SourceFluxBuilder
    └── TurbulenceFluxBuilder
```

### Additional operators

The `Operator` abstraction allows additional matrix-assembly components to be introduced without changing the fundamental flux representation.

For example:

```text
Operator
    │
    ├── DiffusionOperator
    ├── ConvectionOperator
    └── FutureOperator
```

---

# 10. Design Goals

The discretization framework is being developed around several architectural goals:

### Face-based discretization

Mesh connectivity and physical fluxes are represented primarily through faces rather than hard-coded dimensional stencils.

### Separation of construction and assembly

Flux construction and matrix assembly are distinct stages.

### Scheme independence

Operators depend on scheme abstractions rather than specific numerical schemes.

### Physics independence

The discretization layer obtains physical properties and flux quantities through `PhysicsModel` rather than embedding physical models directly.

### Boundary-condition independence

Boundary-condition handling occurs during flux construction, allowing the resulting contributions to enter the common assembly pathway.

### Reusable intermediate representation

`FluxAccumulator` provides a common intermediate representation between physical/discretization calculations and algebraic assembly.

### Extensibility

New discretization schemes, operators, and flux mechanisms should be addable without requiring changes to unrelated components.

---

# 11. Current Status

The finite-volume framework currently supports:

* Face-based diffusion discretization.
* Face-based convection flux construction.
* Upwind convection.
* Central-difference convection.
* Cell-centered source terms.
* Dirichlet boundary conditions.
* Neumann boundary conditions.
* Convective/Robin boundary conditions.
* Manufactured-solution verification forcing.
* Generic operator abstraction.
* Flux accumulation and reuse.
* Separation of flux construction from matrix assembly.
* Green-Gauss gradient reconstruction.
* Least-squares gradient reconstruction.

The current verification suite demonstrates second-order convergence for the applicable diffusion, convection-diffusion, Neumann, and convective cases.

The next architectural targets are:

```text
Finite Volume Framework
    │
    ├── [x] Generic face-based assembly
    ├── [x] Boundary condition framework
    ├── [x] Face abstraction
    ├── [x] Scalar field storage
    ├── [x] Vector field storage
    ├── [x] Field registry infrastructure
    ├── [x] Simulation integration with FieldRegistry
    ├── [x] Generic operator framework
    ├── [ ] Generic equation system abstraction
    └── [ ] Matrix/vector assembly interface
```

These future abstractions should build on the existing separation between discretization, operator assembly, and the linear-system layer rather than collapsing those responsibilities back together.
