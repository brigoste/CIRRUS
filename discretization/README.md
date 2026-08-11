# Discretization Framework

The `discretization/` module contains the finite-volume discretization framework used by CIRRUS.

Its primary responsibility is to transform physical-model information, mesh geometry, and boundary conditions into discrete flux contributions and ultimately into a linear system suitable for solution.

The framework is deliberately divided into two major stages:

1. **Flux construction** — determine the physical/discretization contributions at faces and cells.
2. **Operator assembly** — convert those contributions into matrix and right-hand-side terms.

This separation keeps the discretization logic independent of the details of the linear-system implementation.

---

# Directory Structure

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
├── diffusion/
│   ├── DiffusionScheme.hpp
│   └── StandardDiffusionScheme.cpp
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
│   ├── DiffusionOperator.cpp
│   ├── DiffusionOperator.hpp
│   └── Operator.hpp
│
├── reconstructors/
│   ├── CentralReconstruction.cpp
│   ├── CentralReconstruction.hpp
│   ├── GradientReconstruction.cpp
│   ├── GradientReconstruction.hpp
│   ├── ReconstructionFactory.cpp
│   ├── ReconstructionFactory.hpp
│   ├── ReconstructionScheme.hpp
│   ├── ReconstructionStencil.hpp
│   ├── ReconstructionType.hpp
│   ├── UpwindReconstruction.cpp
│   └── UpwindReconstruction.hpp
│
├── CellResidual.hpp
├── FaceConvection.hpp
├── FaceDiffusion.hpp
├── FaceType.hpp
├── FiniteVolumeAssembler.cpp
├── FiniteVolumeAssembler.hpp
├── FluxAccumulator.hpp
├── FluxBuilder.cpp
└── FluxBuilder.hpp
```

The important architectural change is that **convection schemes are reconstruction schemes**. The face convective flux is constructed independently and the selected reconstruction determines how that flux is distributed into matrix coefficients.

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
                       │ FiniteVolumeAssembler   │
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
                   │                     ┌─────────────────────┐
                   │                     │ReconstructionScheme │
                   │                     └──────────┬──────────┘
                   │                                │
                   │                    ┌───────────┴───────────┐
                   │                    │                       │
                   │                    ▼                       ▼
                   │             ┌──────────────┐       ┌──────────────┐
                   │             │   Gradient   │       │   Central    │
                   │             │ Reconstruction│       │ Reconstruction│
                   │             └──────────────┘       └──────────────┘
                   │
                   │                    ┌──────────────┐
                   │                    │    Upwind    │
                   │                    │ Reconstruction│
                   │                    └──────────────┘
                   │
                   └──────────────┬────────────────┘
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

The reconstruction layer sits inside the convection assembly path:

```text
FaceConvection
      │
      ▼
ConvectionOperator
      │
      ▼
ReconstructionScheme
      │
      ├── CentralReconstruction
      ├── UpwindReconstruction
      └── GradientReconstruction
```

---

# 1. Flux Construction

## `FluxBuilder`

```text
FluxBuilder.cpp
FluxBuilder.hpp
```

`FluxBuilder` is the top-level coordinator for flux construction.

It does not perform the individual discretization calculations itself. Instead, it delegates to specialized builders:

```text
FluxBuilder
    │
    ├── DiffusionFluxBuilder
    ├── ConvectionFluxBuilder
    └── SourceFluxBuilder
```

The builders write their contributions into the same `FluxAccumulator`.

This provides a single entry point for constructing the complete set of discretization contributions for a simulation.

---

## `builders/`

The builder classes translate mesh, physics, and boundary-condition information into discrete flux contributions.

They **do not assemble the linear system**.

### `DiffusionFluxBuilder`

Constructs diffusion-related flux contributions.

Responsibilities include:

* Iterating over relevant faces.
* Computing face diffusion coefficients.
* Adding interior diffusion couplings.
* Processing Dirichlet boundary conditions.
* Processing Neumann boundary conditions.
* Processing convective/Robin boundary conditions.
* Handling manufactured verification boundary conditions.

For an interior face, diffusion is represented by a coupling between the owner and neighbor cells:

```text
P <──── D ────> N
```

Boundary contributions are represented through the cell residual information stored in `FluxAccumulator`.

---

### `ConvectionFluxBuilder`

Constructs convection flux information for interior faces.

For each interior face it obtains the physical face flux and stores:

```text
(P, N, F)
```

in the `FluxAccumulator`.

Here:

* `P` is the owner cell.
* `N` is the neighbor cell.
* `F` is the face convective flux.

The builder does **not** decide how `F` is discretized.

That decision belongs to the selected `ReconstructionScheme`.

This separation allows the same physical face flux to be used by multiple reconstruction strategies.

---

### `SourceFluxBuilder`

Constructs cell-centered source contributions.

For each cell it:

1. Obtains the physical source from the `PhysicsModel`.
2. Multiplies the source by the cell volume.
3. Adds the resulting contribution to `Su`.
4. Adds manufactured verification forcing when a verification case requires it.

The builder therefore handles both physical sources and verification-specific forcing without coupling the rest of the discretization framework directly to the verification system.

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

The accumulator is reset and reused for each assembly:

```cpp
flux.reset();
```

This avoids repeatedly reconstructing the entire data structure during iterative simulation assembly.

`FluxAccumulator` is intentionally non-copyable and supports move semantics to avoid accidental copying of potentially large flux data.

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
* `Sp` is the implicit linearized source contribution.

The final assembly converts these values into linear-system contributions.

---

## `FaceDiffusion`

```text
FaceDiffusion.hpp
```

Represents a diffusion contribution associated with a face.

It contains the owner, neighbor, and diffusion coefficient/conductance:

```text
P
N
D
```

where:

* `P` is the owner cell.
* `N` is the neighboring cell.
* `D` is the diffusion coefficient.

Face information is used by the diffusion operator to construct the corresponding matrix coupling.

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
face
```

where:

* `P` is the owner cell.
* `N` is the neighboring cell.
* `F` is the face convective flux.
* `face` identifies the corresponding mesh face.

The stored `F` represents the physical face flux. Its conversion into matrix coefficients is performed by the selected reconstruction scheme.

This distinction is important:

```text
ConvectionFluxBuilder
        │
        │ determines physical F
        ▼
FaceConvection
        │
        │ discretizes F
        ▼
ReconstructionScheme
        │
        ▼
Matrix contributions
```

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

This provides explicit classification of face-based discretization data.

---

# 4. Reconstruction

The reconstruction layer defines how a cell-centered field is represented at a face.

The central abstraction is:

```text
ReconstructionScheme
```

A reconstruction may provide a **stencil**:

```text
φ_f = Σ w_i φ_i
```

where the weights describe how neighboring cell values contribute to the face value.

The same abstraction can also directly evaluate the reconstructed face value.

---

## `ReconstructionStencil`

```text
reconstructors/ReconstructionStencil.hpp
```

Stores the coefficients of a cell-value reconstruction:

```cpp
struct ReconstructionStencil
{
    std::vector<std::pair<std::size_t, double>> weights;
};
```

For example, a two-cell reconstruction may produce:

```text
cell P       weight wP
cell N       weight wN
```

such that:

```text
φ_f = wP φ_P + wN φ_N
```

The stencil provides a reusable representation that can be consumed by the convection operator.

---

## `ReconstructionScheme`

```text
reconstructors/ReconstructionScheme.hpp
```

Defines the common reconstruction interface.

The primary operation is:

```cpp
virtual ReconstructionStencil stencil(
    const MeshBase& mesh,
    std::size_t owner,
    const Face& face,
    const ScalarField& field,
    const VectorField& gradient
) const;
```

The base implementation rejects schemes that do not provide a cell-value stencil.

The class also provides a default `reconstruct()` implementation that evaluates the returned stencil:

```text
ReconstructionStencil
        │
        ▼
Σ w_i φ_i
        │
        ▼
      φ_f
```

This means a scheme only needs to implement the stencil when its reconstruction can be expressed as a linear combination of cell values.

---

# 5. Reconstruction Schemes

The current reconstruction implementations are:

```text
reconstructors/
├── GradientReconstruction
├── CentralReconstruction
└── UpwindReconstruction
```

They are selected through:

```text
ReconstructionType
```

and constructed by:

```text
ReconstructionFactory
```

---

## `CentralReconstruction`

Central reconstruction represents the face value using the owner and neighbor cells.

For an interior face:

```text
φ_f = (1 - α) φ_P + α φ_N
```

where `α` is determined from the face location relative to the owner-neighbor center-to-center direction.

The resulting stencil is:

```text
P → (1 - α)
N → α
```

This allows the reconstruction to remain valid on nonuniform and non-orthogonal meshes rather than assuming a fixed `α = 0.5`.

For a linear manufactured field, central reconstruction is exact within the tested interior-face configuration.

---

## `UpwindReconstruction`

Upwind reconstruction selects the upstream cell based on the sign of the face convective flux `F`.

Conceptually:

```text
F > 0:

P ─────────► N

φ_f = φ_P
```

and:

```text
F < 0:

P ◄───────── N

φ_f = φ_N
```

Thus the reconstruction depends on the **direction of the physical face flux**.

The important separation is:

```text
ConvectionFluxBuilder
        │
        │ computes F
        ▼
FaceConvection
        │
        │ passes F
        ▼
UpwindReconstruction
        │
        │ selects P or N
        ▼
ReconstructionStencil
```

The reconstruction therefore does not compute the physical convection flux itself. It uses the flux to determine which cell is upwind.

---

## `GradientReconstruction`

Gradient reconstruction evaluates a face value using the owner-cell value and the reconstructed cell gradient.

Conceptually:

```text
φ_f = φ_P + ∇φ_P · (x_f - x_P)
```

The gradient is supplied separately through the active gradient scheme.

This provides a higher-order reconstruction pathway without coupling gradient calculation directly to the convection operator.

---

# 6. Reconstruction Factory

```text
reconstructors/ReconstructionFactory.cpp
reconstructors/ReconstructionFactory.hpp
reconstructors/ReconstructionType.hpp
```

`ReconstructionFactory` constructs the selected reconstruction strategy:

```text
ReconstructionType
        │
        ├── Gradient ──► GradientReconstruction
        ├── Central  ──► CentralReconstruction
        └── Upwind   ──► UpwindReconstruction
```

The active reconstruction is configured through the simulation configuration:

```text
discretization.reconstructionScheme
```

This allows verification cases and simulations to select different reconstruction strategies without changing the finite-volume assembly code.

---

# 7. Operators

The operator layer converts accumulated flux information into linear-system contributions.

The fundamental abstraction is `Operator`.

## `Operator`

```text
operators/Operator.hpp
```

Defines the common operator interface for matrix assembly.

An operator consumes discretization information and contributes to the final algebraic system.

The operator layer does not determine the physical fluxes themselves.

---

## `DiffusionOperator`

```text
operators/DiffusionOperator.cpp
operators/DiffusionOperator.hpp
```

`DiffusionOperator` performs matrix assembly for diffusion contributions.

For an interior face coupling:

```text
       P       N
P     +D      -D
N     -D      +D
```

or equivalently:

```cpp
A[P,P] += D;
A[P,N] -= D;
A[N,N] += D;
A[N,P] -= D;
```

The construction of `D` occurs earlier in `DiffusionFluxBuilder`.

Thus:

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
        │ assembles coefficients
        ▼
LinearSystem
```

---

## `ConvectionOperator`

```text
operators/ConvectionOperator.cpp
operators/ConvectionOperator.hpp
```

`ConvectionOperator` assembles stored convection fluxes.

For each interior face it obtains:

```text
F = FaceConvection.F
```

and passes the face information and flux to the configured `ReconstructionScheme`.

The reconstruction returns a stencil:

```text
φ_f = Σ w_i φ_i
```

The convection operator then converts that stencil into matrix contributions:

```text
coefficient = F w_i
```

and applies the contribution to both owner and neighbor equations.

Conceptually:

```text
FaceConvection
      │
      │ F
      ▼
ConvectionOperator
      │
      ▼
ReconstructionScheme
      │
      ├── Upwind
      ├── Central
      └── Gradient
      │
      ▼
ReconstructionStencil
      │
      ▼
Matrix contributions
```

This means `ConvectionOperator` does not need to know which reconstruction scheme is active.

---

# 8. Finite-Volume Assembly

## `FiniteVolumeAssembler`

```text
FiniteVolumeAssembler.cpp
FiniteVolumeAssembler.hpp
```

`FiniteVolumeAssembler` is the high-level finite-volume assembly coordinator.

Its current sequence is:

```text
1. Diffusion
2. Convection
3. Sources / implicit terms
```

Conceptually:

```text
FluxAccumulator
      │
      ├── diffusion ──────► DiffusionOperator
      │
      ├── convection ─────► ConvectionOperator
      │                              │
      │                              ▼
      │                    ReconstructionScheme
      │
      └── Su / Sp ─────────► LinearSystem
```

For cell-centered source contributions:

```cpp
sys.addRHS(c, flux[c].Su);
sys.addCoeff(c, c, -flux[c].Sp);
```

`FiniteVolumeAssembler` therefore provides the common finite-volume assembly pathway without requiring individual operators to know about one another.

---

# 9. Gradient Schemes

The gradient subsystem remains separate from the primary flux-construction layer.

```text
gradient/
├── GradientScheme.hpp
├── GradientType.hpp
├── GreenGaussGradient.cpp
├── GreenGaussGradient.hpp
├── LeastSquaresGradient.cpp
└── LeastSquaresGradient.hpp
```

## `GradientScheme`

Defines the common interface for cell-gradient calculation.

Current implementations include:

```text
GradientScheme
    │
    ├── GreenGaussGradient
    └── LeastSquaresGradient
```

The gradient schemes provide cell gradients that can be consumed by reconstruction schemes that require gradient information.

For example:

```text
GradientScheme
      │
      ▼
∇φ_P
      │
      ▼
GradientReconstruction
      │
      ▼
φ_f
```

Gradient calculation is therefore a dependency of specific reconstruction strategies rather than an unconditional operation performed by the finite-volume assembler.

This keeps the current solve path free of unnecessary gradient dependencies when a reconstruction such as Upwind or Central does not require them.

---

# 10. Current Assembly Sequence

A simulation assembly proceeds conceptually as follows.

## Step 1 — Reset

The `FluxAccumulator` is cleared:

```text
FluxAccumulator.reset()
```

This removes contributions from the previous assembly.

---

## Step 2 — Construct fluxes

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

## Step 3 — Assemble diffusion

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

## Step 4 — Assemble convection

`ConvectionOperator` reads the stored convection fluxes.

For each face:

```text
F = FaceConvection.F
```

The selected reconstruction receives `F` and determines the face-value stencil.

For example:

```text
Upwind:

F > 0  → φ_f = φ_P
F < 0  → φ_f = φ_N
```

or:

```text
Central:

φ_f = (1 - α)φ_P + αφ_N
```

The resulting stencil is converted into matrix coefficients.

---

## Step 5 — Assemble sources and implicit terms

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

## Step 6 — Solve

After assembly, the resulting `LinearSystem` is passed to the selected linear solver.

The discretization layer itself does not solve the system.

---

# 11. Separation of Responsibilities

The current architecture intentionally separates several concerns.

| Component                | Responsibility                                |
| ------------------------ | --------------------------------------------- |
| `FluxBuilder`            | Coordinates flux construction                 |
| `DiffusionFluxBuilder`   | Constructs diffusion flux data                |
| `ConvectionFluxBuilder`  | Constructs physical convection flux data      |
| `SourceFluxBuilder`      | Constructs source data                        |
| `FluxAccumulator`        | Stores intermediate discretization data       |
| `DiffusionOperator`      | Assembles diffusion matrix contributions      |
| `ConvectionOperator`     | Coordinates convection matrix assembly        |
| `ReconstructionScheme`   | Determines face-value reconstruction          |
| `UpwindReconstruction`   | Selects upstream cell from flux direction     |
| `CentralReconstruction`  | Computes central face-value weights           |
| `GradientReconstruction` | Reconstructs face values using cell gradients |
| `GradientScheme`         | Computes cell gradients                       |
| `FiniteVolumeAssembler`  | Coordinates complete FV matrix assembly       |
| `LinearSystem`           | Stores the final discrete algebraic system    |

The key principle is:

> **Builders determine the physical/discretization data; reconstruction schemes determine how face values are represented; operators determine how those contributions enter the algebraic system.**

---

# 12. Verification

The discretization framework is accompanied by verification tests for the numerical components.

Current verification coverage includes:

* Green-Gauss gradient accuracy.
* Least-squares gradient accuracy.
* Gradient mesh-refinement order.
* Central reconstruction accuracy.
* Gradient reconstruction accuracy.
* Upwind reconstruction integration.
* Face-based diffusion assembly.
* Face-based convection assembly.
* Manufactured-solution forcing.
* Boundary-condition handling.
* Linear-system assembly.

The current verification results demonstrate approximately second-order convergence for the applicable gradient and reconstruction verification cases.

For example, the current central reconstruction verification produces approximately second-order convergence:

```text
10 -> 20    2.0268
20 -> 40    2.0126
40 -> 80    2.0061
```

Upwind reconstruction is intentionally first-order and is not expected to exhibit the same second-order behavior.

A representative current verification case is:

```text
Reconstruction scheme: Upwind

Case                Solver  Reconstruction  Gradient    Mesh    L2 Error
Sinusoidal2D        GMRES   Upwind          green_gauss 50x50   1.645e-04
```

The verification framework can override the discretization configuration on a per-case basis, allowing different reconstruction and gradient schemes to be tested without modifying the base simulation configuration.

---

# 13. Extension Points

The current structure is intended to support future extensions without requiring major changes to the existing assembly framework.

## Additional reconstruction schemes

New convection/face-value schemes can be added under:

```text
reconstructors/
```

For example:

```text
reconstructors/
├── ...
├── QUICKReconstruction
├── MUSCLReconstruction
└── HigherOrderReconstruction
```

These implement `ReconstructionScheme` and can be selected through `ReconstructionType`.

The `ConvectionOperator` does not need to change.

---

## Additional gradient schemes

Additional gradient algorithms can implement `GradientScheme`:

```text
gradient/
├── ...
├── WeightedLeastSquaresGradient
└── ...
```

These can then be consumed by gradient-dependent reconstruction schemes.

---

## Additional physical fluxes

The builder pattern can be extended with specialized builders if future governing equations require additional physical contributions.

For example:

```text
FluxBuilder
    │
    ├── DiffusionFluxBuilder
    ├── ConvectionFluxBuilder
    ├── SourceFluxBuilder
    └── TurbulenceFluxBuilder
```

---

## Additional operators

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

# 14. Design Goals

The discretization framework is being developed around several architectural goals.

### Face-based discretization

Mesh connectivity and physical fluxes are represented primarily through faces rather than hard-coded dimensional stencils.

### Separation of construction and assembly

Flux construction and matrix assembly are distinct stages.

### Reconstruction independence

The convection operator depends on the `ReconstructionScheme` abstraction rather than specific numerical schemes.

### Physics independence

The discretization layer obtains physical properties and flux quantities through the `PhysicsModel` rather than embedding physical models directly into the discretization algorithms.

### Boundary-condition independence

Boundary-condition handling occurs during flux construction, allowing resulting contributions to enter the common assembly pathway.

### Reusable intermediate representation

`FluxAccumulator` provides a common intermediate representation between physical/discretization calculations and algebraic assembly.

### Selective gradient dependency

Gradient calculation is only required when the active reconstruction or discretization actually needs it.

### Extensibility

New reconstruction schemes, gradient schemes, operators, and flux mechanisms should be addable without requiring changes to unrelated components.

---

# 15. Current Status

The finite-volume framework currently supports:

* Face-based diffusion discretization.
* Face-based convection flux construction.
* Upwind reconstruction.
* Central reconstruction.
* Gradient-based reconstruction.
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
* Configurable reconstruction schemes.
* Per-verification-case discretization overrides.

The current architecture is:

```text
Physical Model
      │
      ▼
Flux Builders
      │
      ▼
FluxAccumulator
      │
      ├──────────────┐
      ▼              ▼
Diffusion        Convection
Operator         Operator
                     │
                     ▼
             ReconstructionScheme
                │    │    │
                ▼    ▼    ▼
             Gradient Central Upwind
                │
                ▼
             LinearSystem
                  │
                  ▼
                Solver
```

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
    ├── [x] Configurable reconstruction framework
    ├── [x] Upwind reconstruction
    ├── [x] Central reconstruction
    ├── [x] Gradient reconstruction
    ├── [x] Gradient scheme abstraction
    ├── [x] Verification discretization overrides
    ├── [ ] Generic equation system abstraction
    └── [ ] Matrix/vector assembly interface
```

These future abstractions should build on the existing separation between flux construction, reconstruction, operator assembly, and the linear-system layer rather than collapsing those responsibilities back together.
