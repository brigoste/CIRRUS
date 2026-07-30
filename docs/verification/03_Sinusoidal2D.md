# 03_Sinusoidal2D

## Purpose

This case verifies the implementation of two-dimensional steady diffusion within the heat conduction model. The problem uses a manufactured sinusoidal temperature field over a two-dimensional square domain with constant thermal conductivity.

Unlike the previous one-dimensional verification cases, this problem evaluates the solver's ability to correctly assemble and solve a multidimensional diffusion operator. The smooth analytical solution provides a controlled environment for evaluating spatial discretization accuracy and mesh refinement behavior.

This case verifies:

* two-dimensional diffusion operator implementation
* x-direction and y-direction flux calculations
* finite-volume discretization on a structured quadrilateral mesh
* Dirichlet boundary condition enforcement
* linear system assembly
* iterative solver functionality
* second-order spatial convergence

---

## Governing Equation

The governing equation for steady two-dimensional heat diffusion is:

$$ -\nabla \cdot (k\nabla T)=q''' $$

where:

* $(k)$: thermal conductivity $(W/(m \cdot K))$
* $(x,y)$: spatial coordinates $(m)$
* $(T)$: temperature field $(^\circ C)$
* $(q''')$: manufactured source term $(W/m^3)$

For constant thermal conductivity, the equation becomes:

$$ -k\nabla^2T=q''' $$

where the Laplacian operator is:

$$ \nabla^2T= \frac{\partial^2T}{\partial x^2} + \frac{\partial^2T}{\partial y^2} $$

The manufactured source term is selected such that the analytical solution satisfies this equation exactly.

---

## Domain

The problem uses a two-dimensional computational domain.

The domain is defined as:

$$ 0 \leq x \leq L_x $$

$$ 0 \leq y \leq L_y $$

For this verification case:

| Parameter | Value |
|---|---|
| Domain length ($L_x$) | 1.0 m |
| Domain height ($L_y$) | 1.0 m |
| Mesh type | Uniform quadrilateral mesh |
| Cells | $50 \times 50$ |

The mesh is generated using evenly spaced quadrilateral cells. Temperature is stored at the center of each finite volume.

---

## Boundary Conditions

Dirichlet boundary conditions are applied on all four physical boundaries.

The manufactured solution naturally evaluates to zero on all domain boundaries, allowing a consistent fixed-temperature boundary condition.

The boundary conditions are:

$$ T(0,y)=0 $$

$$ T(L_x,y)=0 $$

$$ T(x,0)=0 $$

$$ T(x,L_y)=0 $$

The boundary conditions are assigned through boundary groups:

| Group | Boundary | Type | Value |
|---|---|---|---|
| 0 | Left | Dirichlet | $0^\circ C$ |
| 1 | Right | Dirichlet | $0^\circ C$ |
| 2 | Bottom | Dirichlet | $0^\circ C$ |
| 3 | Top | Dirichlet | $0^\circ C$ |

---

## Source Term

The manufactured solution requires a spatially varying source term.

The source is calculated from:

$$ q'''=-k\nabla^2T $$

The thermal conductivity is constant:

$$ k=100 $$

The resulting source distribution varies throughout the domain according to the analytical solution.

Unlike the previous Quadratic1D case, the source is not uniform. This allows verification of spatially varying source-term integration within the finite-volume formulation.

---

## Manufactured Solution

The analytical temperature distribution is defined as:

$$ T(x,y)= \sin(\pi x)\sin(\pi y) $$

This solution satisfies all boundary conditions because:

$$ \sin(0)=0 $$

and:

$$ \sin(\pi)=0 $$

The second derivatives are:

$$ \frac{\partial^2T}{\partial x^2} = -\pi^2 \sin(\pi x)\sin(\pi y) $$


$$ \frac{\partial^2T}{\partial y^2} = -\pi^2 \sin(\pi x)\sin(\pi y) $$

Therefore, the Laplacian is:

$$ \nabla^2T = -2\pi^2 \sin(\pi x)\sin(\pi y) $$

The manufactured source term is therefore:

$$ q''' = -k\nabla^2T $$

or:

$$ q''' = 2k\pi^2 \sin(\pi x)\sin(\pi y) $$

This expression is identical to the analytical solution implemented in `Sinusoidal2D::exact()` and the source calculation implemented in `Sinusoidal2D::source()`.

The analytical solution is used as the reference solution for error evaluation.

---

## Discretization Details

The Sinusoidal2D verification case uses a cell-centered finite-volume discretization of the steady two-dimensional heat equation.

The governing equation is integrated over each control volume:

$$ -\int_{V_P} \nabla\cdot(k\nabla T)dV = \int_{V_P}q'''dV $$

Applying the divergence theorem converts the volume integral into a balance of diffusive fluxes through the cell faces:

$$ -\sum_f kA_f \frac{\partial T}{\partial n}\bigg|_f = q'''V_P $$

### Spatial Discretization

The domain is divided into uniform quadrilateral finite volumes.

Temperature is stored at cell centers:

$$ T_P=T(x_P,y_P) $$

where $P$ represents the cell center.

Fluxes are evaluated independently in the x and y directions.

### Flux Approximation

Diffusive face gradients are approximated using second-order central differences:

$$ \frac{\partial T}{\partial x}\bigg|_f \approx \frac{T_E-T_P}{d_{PE}} $$


$$ \frac{\partial T}{\partial y}\bigg|_f \approx \frac{T_N-T_P}{d_{PN}} $$

where:

* $T_P$: current cell temperature
* $T_E$: east neighboring cell temperature
* $T_N$: north neighboring cell temperature
* $d$: distance between cell centers

The resulting finite-volume equation is assembled as:

$$ a_PT_P = a_ET_E + a_WT_W + a_NT_N + a_ST_S + b_P $$

where $b_P$ contains the integrated source contribution.

Because the mesh is uniform and orthogonal, the discretization is formally second-order accurate.

---

## Solver Configuration

The assembled diffusion matrix is symmetric positive definite and is solved using the Conjugate Gradient (CG) solver.

| Parameter | Value |
|---|---|
| Solver | CG |
| Tolerance | $1e^{-8}$ |
| Maximum iterations | 5000 |

CG convergence depends on the conditioning of the assembled matrix and the selected convergence tolerance.

---

## Verification Procedure

The numerical solution is compared against the manufactured analytical solution.

The following error metrics are calculated.

### L2 Norm

$$ L_2= \sqrt{ \frac{1}{N} \sum_{i=1}^{N} (T_i-T_i^{exact})^2 } $$

### L∞ Norm

$$ L_\infty= \max_i |T_i-T_i^{exact}| $$

A mesh refinement study is also performed to verify spatial convergence.

For a second-order discretization:

$$ L_2 \propto \Delta x^2 $$

---

## Expected Results

The finite-volume discretization is expected to demonstrate second-order spatial convergence.

The refinement study uses:

| Mesh | Expected Order |
|---|---|
| $25\times25$ | - |
| $50\times50$ | 2 |
| $100\times100$ | 2 |
| $200\times200$ | 2 |

The computed results are:

| Mesh | $L_2$ Error | $L_\infty$ Error |
|---|---|---|
| $25\times25$ | $6.585e^{-4}$ | $1.317e^{-3}$ |
| $50\times50$ | $1.645e^{-4}$ | $3.287e^{-4}$ |
| $100\times100$ | $4.113e^{-5}$ | $8.223e^{-5}$ |
| $200\times200$ | $1.028e^{-5}$ | $2.056e^{-5}$ |

The observed convergence rates are:

| Metric | Observed Order | Expected Order | Status |
|---|---|---|---|
| $L_2$ | 2.00037 | 2.0 | PASS |
| $L_\infty$ | 2.00040 | 2.0 | PASS |

The refinement study confirms that the two-dimensional diffusion operator maintains the expected second-order spatial accuracy.

---

## Components Verified

The Sinusoidal2D case exercises the following components:

| Component | Tested |
|---|---|
| 2D mesh generation | ✓ |
| Cell-centered finite volume discretization | ✓ |
| x-direction diffusion flux calculation | ✓ |
| y-direction diffusion flux calculation | ✓ |
| Spatially varying source integration | ✓ |
| Dirichlet boundary conditions | ✓ |
| Linear system assembly | ✓ |
| CG solver | ✓ |
| Mesh refinement framework | ✓ |
| Analytical solution comparison | ✓ |
| Verification error calculation | ✓ |

---

## Input Configuration

The following configuration was used:

```json
{
  "extends": "../base.json",

  "verificationCase": {
    "name": "Sinusoidal2D",
    "type": "manufactured",

    "plot_enabled": true,

    "mesh": {
      "type": "quad2D",
      "nx": 50,
      "ny": 50,
      "lx": 1.0,
      "ly": 1.0
    },

    "refinement": {
      "enabled": true,
      "levels": [25, 50, 100, 200],
      "expected_order": 2.0
    },

    "physics": {
      "type": "heat",
      "gamma": 1.0,
      "ux": 0.0,
      "uy": 0.0
    },

    "solver": {
      "type": "CG",
      "tol": 1e-8,
      "max_iter": 5000
    },

    "boundary_conditions": [
      {
        "group": 0,
        "type": "Dirichlet",
        "value": 0.0
      },
      {
        "group": 1,
        "type": "Dirichlet",
        "value": 0.0
      },
      {
        "group": 2,
        "type": "Dirichlet",
        "value": 0.0
      },
      {
        "group": 3,
        "type": "Dirichlet",
        "value": 0.0
      }
    ],

    "norms": {
      "l2": true,
      "linf": true
    },

    "output": {
      "csv": "Sinusoidal2D.csv",
      "summary": "Sinusoidal2D.json"
    }
  }
}
