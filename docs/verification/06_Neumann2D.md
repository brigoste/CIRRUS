# Neumann2D Verification Case

## Overview

The `Neumann2D` verification case validates the implementation of Neumann boundary conditions for the 2D steady heat equation.

The case uses a manufactured quadratic temperature field with:
- a Dirichlet boundary condition on one boundary to provide a reference temperature
- Neumann heat flux boundary conditions on the remaining boundaries
- a constant volumetric source term

The Neumann flux is evaluated analytically on each boundary face using the local face geometry. This verifies that boundary fluxes are correctly applied for spatially varying conditions.

This case verifies:
- correct application of face-based Neumann boundary fluxes
- correct use of face normals and boundary geometry
- correct incorporation of volumetric source terms
- second-order spatial accuracy of the diffusion discretization

---

## Governing Equation

The steady 2D heat equation is:

$$
-k\nabla^2 T=q'''
$$

where:

- $(k)$ is the thermal conductivity
- $(q''')$ is the volumetric heat source
- $(T)$ is the temperature field

The Laplacian operator is:

$$
\nabla^2 T =
\frac{\partial^2 T}{\partial x^2}
+
\frac{\partial^2 T}{\partial y^2}
$$

---

## Boundary Conditions

The manufactured solution applies Neumann boundary conditions through the normal heat flux:

$$
q_n=-k\nabla T\cdot \mathbf{n}
$$

where:

- $q_n$ is the outward normal heat flux
- $\nabla T$ is the analytical temperature gradient
- $\mathbf{n}$ is the outward face normal

For each boundary face, CIRRUS evaluates the analytical flux using the face center and normal vector.

The resulting finite-volume source contribution is:

$$
S_u=-q_n A_f
$$

where:

- $A_f$ is the boundary face area

This formulation allows spatially varying Neumann conditions because each face independently evaluates its boundary flux.

---

## Manufactured Solution

The manufactured temperature field is:

$$
T(x,y)=T_0
+a x
+b y
+c x^2
+d y^2
$$

where:

- $T_0$ is the reference temperature
- $a,b$ define the linear temperature gradients
- $c,d$ define the quadratic curvature terms

The temperature gradients are:

$$
\frac{\partial T}{\partial x}=a+2cx
$$

$$
\frac{\partial T}{\partial y}=b+2dy
$$

The Laplacian is:

$$
\nabla^2T=2c+2d
$$

which satisfies the governing equation through:

$$
q'''=-k(2c+2d)
$$

---

## Analytical Boundary Flux

For each boundary face:

$$
q_n=-k
\left(
\frac{\partial T}{\partial x}n_x
+
\frac{\partial T}{\partial y}n_y
\right)
$$

or:

$$
q_n=-k
\left(
(a+2cx)n_x
+
(b+2dy)n_y
\right)
$$

The boundary flux is computed using the face center location and outward normal vector stored in the mesh geometry.

---

## Validation Criteria

The Neumann2D case is considered valid when:

| Criterion | Requirement |
|---|---:|
| L2 error | <= 5e-2 |
| L∞ error | <= 5e-2 |
| Spatial convergence order | 2.0 |
| Refinement study | PASS |

The acceptance threshold of $5\times10^{-2}$ was selected based on the expected truncation error of the second-order finite-volume diffusion discretization on the coarsest verification mesh. The tolerance provides sufficient margin for numerical implementation details while remaining sensitive to incorrect boundary flux application, source-term treatment, or loss of expected spatial convergence.

For the Neumann2D case, the computational domain is a unit square and the coarsest verification mesh spacing is:

$$
h=\frac{L}{N}=\frac{1}{25}=0.04
$$

The expected discretization error scales as:

$$
E=O(h^2)
$$

The refinement study provides the primary verification of discretization accuracy by confirming that the expected second-order convergence behavior is maintained.

The case was validated using the mesh refinement sequence:

$$
N=\{25,50,100,200\}
$$

The observed convergence rates were:

$$
p_{L2}=2.0
$$

$$
p_{L\infty}=2.0
$$

The final refined solution produced:

- Mesh: $200\times200$
- Solver: CG

L2 error:

$$
L_2=3.125\times10^{-4}
$$

L∞ error:

$$
L_{\infty}=3.125\times10^{-4}
$$

Both accuracy and refinement criteria were satisfied.

The final error is more than two orders of magnitude below the acceptance threshold, confirming the correct implementation of the Neumann boundary flux treatment, volumetric source contribution, and second-order spatial discretization.

---

## Verification Parameters

Example configuration:

```json
{
  "extends": "../base.json",

  "verificationCase": {
    "name": "Neumann2D",
    "type": "manufactured",

    "mesh": {
      "type": "quad2D",
      "nx": 200,
      "ny": 200,
      "lx": 1.0,
      "ly": 1.0
    },

    "refinement": {
      "enabled": true,
      "levels": [25,50,100,200],
      "expected_order": 2.0
    },

    "physics": {
      "type": "heat",
      "k": 100.0,
      "volumetricSource": -5000.0
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
        "value": 100.0
      },
      {
        "group": 1,
        "type": "Neumann",
        "flux": 0.0
      },
      {
        "group": 2,
        "type": "Neumann",
        "flux": 0.0
      },
      {
        "group": 3,
        "type": "Neumann",
        "flux": 0.0
      }
    ],

    "norms": {
      "l2": true,
      "linf": true
    },

    "output": {
      "csv": "Neumann2D.csv",
      "summary": "Neumann2D.json"
    }
  }
}
