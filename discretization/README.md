# Discretization Framework

The `discretization/` module contains the finite-volume discretization framework used by CIRRUS.

Its primary responsibility is to transform physical-model information, mesh geometry, and boundary conditions into discrete contributions and ultimately into a linear system suitable for solution.

The framework is deliberately divided into two major stages:

1. **Flux construction** — determine the physical and discretization contributions associated with faces, boundaries, and cells.
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
├── FaceContribution.hpp
├── FaceConvection.hpp
├── FaceDiffusion.hpp
├── BoundaryDiffusion.hpp
├── FaceType.hpp
├── MatrixContribution.hpp
├── FiniteVolumeAssembler.cpp
├── FiniteVolumeAssembler.hpp
├── FluxAccumulator.hpp
├── FluxBuilder.cpp
└── FluxBuilder.hpp
```

The important architectural changes are:

* `FaceConvection` and `FaceDiffusion` now inherit from a common `FaceContribution` base.
* Boundary diffusion is represented explicitly through `BoundaryDiffusion`.
* Matrix entries generated during operator assembly are represented by `MatrixContribution`.
* `FluxAccumulator` acts as the intermediate representation for both physical/discretization data and generated algebraic contributions.
* Convection schemes are reconstruction schemes. The physical convective flux is constructed independently from the choice of reconstruction.

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
                         │     FluxBuilder     │
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
                         │ • face contributions│
                         │ • cell residuals    │
                         │ • matrix entries    │
                         └──────────┬──────────┘
                                    │
                                    ▼
                       ┌────────────────────────┐
                       │ FiniteVolumeAssembler  │
                       │      (orchestrator)     │
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
                   │                    ┌───────────┼───────────┐
                   │                    │           │           │
                   │                    ▼           ▼           ▼
                   │             ┌────────────┐ ┌──────────┐ ┌──────────┐
                   │             │ Gradient   │ │ Central  │ │ Upwind   │
                   │             │Reconstruction│ │Reconstruction│ │Reconstruction│
                   │             └────────────┘ └──────────┘ └──────────┘
                   │
                   └──────────────────┬────────────────────────┐
                                      │                        │
                                      ▼                        ▼
                              Matrix contributions       Cell residuals
                                      │                        │
                                      └───────────┬────────────┘
                                                  ▼
                                        ┌─────────────────────┐
                                        │    LinearSystem     │
                                        │                     │
                                        │    Matrix + RHS     │
                                        └─────────────────────┘
```

The important architectural boundary remains:

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

This provides a single entry point for constructing the complete set of discretization information required for a simulation.

---

## `builders/`

The builder classes translate mesh, physics, and boundary-condition information into discrete contributions.

They **do not directly assemble the final linear system**.

### `DiffusionFluxBuilder`

Constructs diffusion-related contributions.

Responsibilities include:

* Iterating over relevant faces.
* Computing face diffusion coefficients.
* Adding interior diffusion contributions.
* Processing Dirichlet boundary conditions.
* Processing Neumann boundary conditions.
* Processing Robin boundary conditions.
* Handling manufactured verification boundary conditions.

For an interior face, the builder creates a `FaceDiffusion` contribution:

```text
P
N
face
D
```

For a Dirichlet boundary, it creates a `BoundaryDiffusion` contribution.

The boundary contribution is then converted into cell residual terms by `FluxAccumulator`.

---

### `ConvectionFluxBuilder`

Constructs convection flux information for interior faces.

For each interior face it obtains the physical face flux and creates a `FaceConvection` contribution:

```text
P
N
face
F
```

Here:

* `P` is the owner cell.
* `N` is the neighbor cell.
* `F` is the face convective flux.
* `face` identifies the corresponding mesh face.

The builder does **not** decide how `F` is discretized.

That decision belongs to the selected `ReconstructionScheme`.

This separation allows the same physical face flux to be used by multiple reconstruction strategies.

---

### `SourceFluxBuilder`

Constructs cell-centered source contributions.

For each cell it:

1. Obtains the physical source from the `PhysicsModel`.
2. Multiplies the source by the cell volume.
3. Adds the resulting contribution to the cell residual.
4. Adds manufactured verification forcing when required.

The builder therefore handles both physical sources and verification-specific forcing without coupling the rest of the discretization framework directly to the verification system.

---

# 2. Flux Storage

## `FluxAccumulator`

```text
FluxAccumulator.hpp
```

`FluxAccumulator` is the intermediate representation between flux construction and operator/algebraic assembly.

It stores three broad categories of discretization information:

```text
FluxAccumulator
│
├── Face contributions
│   ├── FaceDiffusion
│   ├── FaceConvection
│   └── BoundaryDiffusion
│
├── Cell residuals
│   └── CellResidual
│
└── Matrix contributions
    └── MatrixContribution
```

The accumulator is reset and reused for each assembly:

```cpp
flux.reset();
```

This removes the previous assembly's contributions without requiring the entire accumulator to be reconstructed.

`FluxAccumulator` is intentionally non-copyable and supports move semantics to avoid accidental copying of potentially large discretization data.

The accumulator also provides a common insertion interface for contribution objects. This keeps contribution construction separate from the storage mechanism.

Conceptually:

```text
Builder / Operator
       │
       │ creates contribution
       ▼
FluxAccumulator
       │
       │ stores contribution
       ▼
Assembly
```

---

# 3. Contribution Data Structures

The contribution hierarchy provides a common representation for face-based data.

## `FaceContribution`

```text
FaceContribution.hpp
```

`FaceContribution` contains the common connectivity information shared by face-based contributions:

```cpp
struct FaceContribution
{
    std::size_t P = 0;
    std::size_t N = Face::INVALID;
    std::size_t face = 0;
};
```

Where:

* `P` is the owner cell.
* `N` is the neighbor cell.
* `face` identifies the mesh face.

`Face::INVALID` is used for the neighbor when a face does not have a neighboring cell.

Specialized face contributions extend this common structure.

```text
FaceContribution
       │
       ├── FaceDiffusion
       │      └── D
       │
       └── FaceConvection
              └── F
```

This avoids duplicating the common owner/neighbor/face information in every face contribution type.

---

## `FaceDiffusion`

```text
FaceDiffusion.hpp
```

Represents an interior diffusion contribution associated with a face.

It extends `FaceContribution` with the diffusion coefficient:

```text
P
N
face
D
```

The contribution is constructed by `DiffusionFluxBuilder` and later consumed by `DiffusionOperator`.

---

## `FaceConvection`

```text
FaceConvection.hpp
```

Represents an interior convection contribution.

It extends `FaceContribution` with the physical face flux:

```text
P
N
face
F
```

The stored `F` represents the physical convective flux.

Its conversion into matrix coefficients is performed later by `ConvectionOperator` and the configured reconstruction scheme.

---

## `BoundaryDiffusion`

```text
BoundaryDiffusion.hpp
```

Represents a diffusion contribution associated with a Dirichlet boundary.

It contains the owner cell and the diffusion information required to convert the boundary value into implicit and explicit cell contributions:

```text
P
D
value
```

The contribution is constructed during boundary flux construction and consumed by `FluxAccumulator`.

For a Dirichlet boundary, the resulting terms are conceptually:

```text
Sp -= D
Su += D φ_boundary
```

This makes the boundary treatment explicit rather than embedding the boundary calculation directly into the final matrix assembly.

---

## `MatrixContribution`

```text
MatrixContribution.hpp
```

Represents a single algebraic matrix entry contribution:

```cpp
struct MatrixContribution
{
    std::size_t row = 0;
    std::size_t column = 0;
    double coefficient = 0.0;
};
```

A matrix contribution represents:

```text
A[row,column] += coefficient
```

This provides a common intermediate representation for operator-generated matrix terms.

For example, standard diffusion produces:

```text
(P,P,+D)
(P,N,-D)
(N,N,+D)
(N,P,-D)
```

The operator does not need to know how those entries are ultimately stored in the `LinearSystem`.

---

# 4. Cell Residual

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

These terms eventually become linear-system contributions:

```text
Su → RHS
Sp → diagonal coefficient
```

---

# 5. Reconstruction

The reconstruction layer defines how a cell-centered field is represented at a face.

The central abstraction is:

```text
ReconstructionScheme
```

A reconstruction may provide a stencil:

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

For example:

```text
cell P → weight wP
cell N → weight wN
```

produces:

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
    const VectorField& gradient,
    double flux
) const;
```

The reconstruction receives the physical face flux because some reconstruction schemes, such as upwind reconstruction, require the flux direction to determine the upstream cell.

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

This means a scheme can implement its reconstruction as a reusable linear stencil.

---

# 6. Reconstruction Schemes

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

This allows the reconstruction to remain valid on nonuniform meshes rather than assuming a fixed `α = 0.5`.

---

## `UpwindReconstruction`

Upwind reconstruction selects the upstream cell based on the sign of the physical face convective flux `F`.

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

The separation is:

```text
ConvectionFluxBuilder
        │
        │ computes F
        ▼
FaceConvection
        │
        │ provides F
        ▼
UpwindReconstruction
        │
        │ selects P or N
        ▼
ReconstructionStencil
```

The reconstruction therefore does not compute the physical convection flux itself.

---

## `GradientReconstruction`

Gradient reconstruction evaluates a face value using the owner-cell value and cell gradient.

Conceptually:

```text
φ_f = φ_P + ∇φ_P · (x_f - x_P)
```

The gradient is supplied separately through the active gradient scheme.

This provides a higher-order reconstruction pathway without coupling gradient calculation directly to the convection operator.

---

# 7. Reconstruction Factory

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

The active reconstruction is configured through the simulation configuration.

This allows verification cases and simulations to select different reconstruction strategies without changing the finite-volume assembly code.

---

# 8. Operators

The operator layer converts accumulated discretization information into algebraic contributions.

The fundamental abstraction is `Operator`.

## `Operator`

```text
operators/Operator.hpp
```

Defines the common operator interface for matrix assembly.

An operator consumes discretization information and generates `MatrixContribution` objects and/or cell residual contributions.

The operator layer does not determine the physical fluxes themselves.

---

## `DiffusionOperator`

```text
operators/DiffusionOperator.cpp
operators/DiffusionOperator.hpp
```

`DiffusionOperator` performs matrix assembly for stored diffusion contributions.

For an interior face:

```text
       P       N
P     +D      -D
N     -D      +D
```

The operator generates the equivalent matrix contributions:

```text
(P,P,+D)
(P,N,-D)
(N,N,+D)
(N,P,-D)
```

Conceptually:

```text
FaceDiffusion
      │
      ▼
DiffusionOperator
      │
      ▼
MatrixContribution
      │
      ▼
LinearSystem
```

The construction of `D` occurs earlier in `DiffusionFluxBuilder`.

This maintains the separation:

```text
DiffusionFluxBuilder → determines D
DiffusionOperator    → determines matrix entries
```

---

## `StandardDiffusionScheme`

```text
diffusion/StandardDiffusionScheme.cpp
diffusion/DiffusionScheme.hpp
```

The diffusion scheme determines how a `FaceDiffusion` contribution is translated into matrix coefficients.

For the standard two-point diffusion discretization:

```text
A[P,P] += D
A[P,N] -= D

A[N,N] += D
A[N,P] -= D
```

The scheme therefore owns the discretization-specific matrix pattern, while `DiffusionOperator` coordinates application of the scheme to the stored face contributions.

Conceptually:

```text
FaceDiffusion
      │
      ▼
DiffusionOperator
      │
      ▼
DiffusionScheme
      │
      ▼
MatrixContribution
```

This provides an extension point for future diffusion discretization schemes without requiring changes to the flux builder.

---

## `ConvectionOperator`

```text
operators/ConvectionOperator.cpp
operators/ConvectionOperator.hpp
```

`ConvectionOperator` assembles stored convection fluxes.

For each face:

```text
F = FaceConvection.F
```

The operator passes the face information and physical flux to the configured `ReconstructionScheme`.

The reconstruction returns a stencil:

```text
φ_f = Σ w_i φ_i
```

The convection operator then converts the stencil into matrix contributions.

For each stencil entry:

```text
coefficient = F w_i
```

and the contribution is applied to both owner and neighbor equations with the appropriate sign.

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
MatrixContribution
```

This means `ConvectionOperator` does not need to know which reconstruction scheme is active.

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

The gradient schemes provide cell gradients that can be consumed by reconstruction schemes requiring gradient information.

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

This keeps the solve path free of unnecessary gradient dependencies when a reconstruction such as Upwind or Central does not require them.

---

# 10. Finite-Volume Assembly

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
4. Apply accumulated matrix contributions
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
      ├── Su / Sp
      │
      └── MatrixContribution
                     │
                     ▼
              LinearSystem
```

The accumulator therefore provides a common intermediate representation regardless of whether a contribution originated from:

* a physical source,
* a boundary condition,
* diffusion,
* convection, or
* a reconstruction-generated matrix coupling.

For cell residual contributions:

```text
Su → RHS
Sp → diagonal coefficient
```

For matrix contributions:

```text
(row, column, coefficient)
        │
        ▼
A[row,column] += coefficient
```

The discretization framework therefore does not require operators to directly manipulate the internal storage of the final linear system.

---

# 11. Current Assembly Sequence

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

with face contributions and cell residual information.

---

## Step 3 — Assemble diffusion

`DiffusionOperator` reads the stored diffusion contributions and passes them through the configured `DiffusionScheme`.

The scheme produces matrix contributions such as:

```text
(P,P,+D)
(P,N,-D)
(N,N,+D)
(N,P,-D)
```

These are stored as `MatrixContribution` objects.

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

F > 0 → φ_f = φ_P
F < 0 → φ_f = φ_N
```

or:

```text
Central:

φ_f = (1 - α)φ_P + αφ_N
```

The resulting stencil is converted into `MatrixContribution` objects.

---

## Step 5 — Apply cell residuals

Cell residual information is converted into algebraic contributions:

```text
Su → RHS
Sp → diagonal coefficient
```

Conceptually:

```text
CellResidual
      │
      ├── Su ──► RHS
      └── Sp ──► diagonal
```

---

## Step 6 — Apply matrix contributions

The accumulated matrix contributions are applied to the final `LinearSystem`:

```text
MatrixContribution
      │
      ▼
LinearSystem
      │
      ▼
A[row,column] += coefficient
```

This provides a clean boundary between the discretization framework and the linear-system implementation.

---

## Step 7 — Solve

After assembly, the resulting `LinearSystem` is passed to the selected linear solver.

The discretization layer itself does not solve the system.

---

# 12. Separation of Responsibilities

The current architecture intentionally separates several concerns.

| Component                | Responsibility                                                     |
| ------------------------ | ------------------------------------------------------------------ |
| `FluxBuilder`            | Coordinates flux construction                                      |
| `DiffusionFluxBuilder`   | Constructs diffusion contributions and handles diffusion BCs       |
| `ConvectionFluxBuilder`  | Constructs physical convection flux data                           |
| `SourceFluxBuilder`      | Constructs source data                                             |
| `FaceContribution`       | Stores common face connectivity                                    |
| `FaceDiffusion`          | Stores interior diffusion face data                                |
| `FaceConvection`         | Stores interior convection face data                               |
| `BoundaryDiffusion`      | Stores Dirichlet diffusion boundary data                           |
| `CellResidual`           | Stores explicit and implicit cell source terms                     |
| `MatrixContribution`     | Represents a single matrix-entry contribution                      |
| `FluxAccumulator`        | Stores intermediate discretization and algebraic contribution data |
| `DiffusionOperator`      | Coordinates diffusion matrix assembly                              |
| `DiffusionScheme`        | Determines diffusion discretization coefficients                   |
| `ConvectionOperator`     | Coordinates convection matrix assembly                             |
| `ReconstructionScheme`   | Determines face-value reconstruction                               |
| `UpwindReconstruction`   | Selects upstream cell from flux direction                          |
| `CentralReconstruction`  | Computes central face-value weights                                |
| `GradientReconstruction` | Reconstructs face values using cell gradients                      |
| `GradientScheme`         | Computes cell gradients                                            |
| `FiniteVolumeAssembler`  | Coordinates complete FV matrix assembly                            |
| `LinearSystem`           | Stores the final discrete algebraic system                         |

The key principle is:

> **Builders determine physical/discretization data; contribution types represent that data; reconstruction schemes determine how face values are represented; operators convert those representations into algebraic contributions; and the finite-volume assembler applies those contributions to the linear system.**

---

# 13. Verification

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

The verification framework can override discretization configuration on a per-case basis, allowing different reconstruction and gradient schemes to be tested without modifying the base simulation configuration.

The current verification results demonstrate approximately second-order convergence for applicable gradient and central-reconstruction cases.

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

---

# 14. Extension Points

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

## Additional diffusion schemes

New diffusion discretizations can implement `DiffusionScheme`.

For example:

```text
DiffusionScheme
    │
    ├── StandardDiffusionScheme
    └── FutureDiffusionScheme
```

The `DiffusionOperator` can remain unchanged while the scheme determines the matrix contribution pattern.

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

## Additional contribution types

The contribution hierarchy can be extended when a discretization mechanism requires information not represented by the current structures.

Face-based contributions should inherit from:

```text
FaceContribution
```

when they share owner/neighbor/face connectivity.

For example:

```text
FaceContribution
       │
       ├── FaceDiffusion
       ├── FaceConvection
       └── FutureFaceContribution
```

This keeps common connectivity centralized while allowing specialized contribution data to remain local to each discretization mechanism.

---

## Additional operators

The `Operator` abstraction allows additional matrix-assembly components to be introduced without changing the fundamental contribution representation.

For example:

```text
Operator
    │
    ├── DiffusionOperator
    ├── ConvectionOperator
    └── FutureOperator
```

---

# 15. Design Goals

The discretization framework is being developed around several architectural goals.

### Face-based discretization

Mesh connectivity and physical fluxes are represented primarily through faces rather than hard-coded dimensional stencils.

### Separation of construction and assembly

Flux construction and matrix assembly are distinct stages.

### Explicit contribution representation

Physical and discretization data are represented through dedicated contribution structures rather than being immediately written into the final linear system.

### Shared face connectivity

Common owner, neighbor, and face information is represented by `FaceContribution` and inherited by specialized face contributions.

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

New reconstruction schemes, gradient schemes, contribution types, operators, and flux mechanisms should be addable without requiring changes to unrelated components.

---

# 16. Current Status

The finite-volume framework currently supports:

* Face-based diffusion discretization.
* Face-based convection flux construction.
* Common `FaceContribution` abstraction.
* `FaceDiffusion` and `FaceConvection` specialized contributions.
* Explicit `BoundaryDiffusion` representation.
* Explicit `MatrixContribution` representation.
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
      ├── FaceContribution
      │     ├── FaceDiffusion
      │     └── FaceConvection
      │
      ├── BoundaryDiffusion
      │
      ├── CellResidual
      │
      └── MatrixContribution
                │
                ▼
        FiniteVolumeAssembler
                │
        ┌───────┴────────┐
        ▼                ▼
   Diffusion         Convection
   Operator           Operator
        │                │
        ▼                ▼
DiffusionScheme  ReconstructionScheme
                         │
                 ┌───────┼───────┐
                 ▼       ▼       ▼
              Gradient Central  Upwind
                 │
                 ▼
          Matrix Contributions
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
    ├── [x] FaceContribution hierarchy
    ├── [x] Boundary diffusion contribution
    ├── [x] MatrixContribution representation
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

These future abstractions should build on the existing separation between flux construction, contribution representation, reconstruction, operator assembly, and the linear-system layer rather than collapsing those responsibilities back together.
