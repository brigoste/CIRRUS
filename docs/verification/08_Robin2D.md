# Robin2D Verification Case

## Overview

The `Robin2D` verification case validates the implementation of mixed thermal boundary conditions for the 2D steady heat equation.

The case represents a rectangular domain with:

* a prescribed temperature (Dirichlet) boundary on the left face
* insulated (Neumann) boundaries on the top and bottom faces
* convective (Robin) heat transfer on the right face

The convective boundary condition introduces heat exchange between the computational domain and an ambient environment through a local surface heat transfer coefficient.

This case verifies:

* correct application of face-based Dirichlet boundary conditions
* correct application of convective (Robin) boundary conditions
* correct treatment of insulated Neumann boundaries
* correct incorporation of volumetric heat sources
* second-order spatial accuracy of the diffusion discretization

---

## Governing Equation

The steady two-dimensional heat equation is:

$$
-k\nabla^2T=q'''
$$

where:

* (k) is the thermal conductivity
* (q''') is the volumetric heat source
* (T) is the temperature field

The Laplacian operator is:

$$
\nabla^2T=
\frac{\partial^2T}{\partial x^2}
+
\frac{\partial^2T}{\partial y^2}
$$

---

## Boundary Conditions

The rectangular domain applies three different boundary condition types.

### Left Boundary: Dirichlet Heating

The left boundary applies a prescribed temperature condition:

$$
T=T(0,y)
$$

where the boundary temperature is obtained from the manufactured solution.

This verifies the application of spatially varying Dirichlet conditions evaluated on boundary faces.

---

### Top and Bottom Boundaries: Insulated Neumann Conditions

The top and bottom boundaries are insulated:

$$
q_n=-k\nabla T\cdot n=0
$$

where:

* $(q_n)$ is the outward normal heat flux
* $(\nabla T)$ is the temperature gradient
* $(n)$ is the outward boundary normal

This verifies that zero-flux boundaries are correctly applied using face geometry and boundary normals.

---

### Right Boundary: Convective Robin Condition

The right boundary exchanges heat with an ambient environment:

$$
-k\nabla T\cdot n=h(T-T_\infty)
$$

where:

* $(h)$ is the convective heat transfer coefficient
* $(T_\infty)$ is the ambient temperature

For this case:

$$
h=10
$$

and:

$$
T_\infty=93
$$

The convective boundary contribution is evaluated locally at each boundary face using the face center location and geometric information.

---

## Manufactured Solution

The manufactured temperature field is:

$$
T(x,y)=T_0+a x+c x^2+d\cos\left(\frac{\pi y}{L_y}\right)
$$

with parameters:

$ T_0=100 $

$ a=5 $

$ c=-2 $

$ d=10 $

$ L_y=1 $

The temperature gradients are:

$$
\frac{\partial T}{\partial x} = a+2cx
$$

and:

$$
\frac{\partial T}{\partial y} =
-d\frac{\pi}{L_y}
\sin\left(\frac{\pi y}{L_y}\right)
$$

The Laplacian is:

$$
\nabla^2T = 2c -

d\left(\frac{\pi}{L_y}\right)^2
\cos\left(\frac{\pi y}{L_y}\right)
$$

The volumetric source is selected so the manufactured solution satisfies:

$$
q'''=-k\nabla^2T
$$

For this verification case:

$$
k=100
$$

and:

$$
q'''=400
$$

---

## Convective Boundary Evaluation

The Robin boundary condition is evaluated on each boundary face:

$$
-k\nabla T\cdot n=h(T-T_\infty)
$$

The analytical temperature and gradient are evaluated at each face center.

The finite-volume discretization incorporates the convective boundary contribution through the boundary face coefficient:

$$
a_f=hA_f
$$

where:

* (A_f) is the boundary face area

This allows the convective boundary condition to vary spatially through the manufactured solution.

---

## Validation Criteria

The `Robin2D` case is considered valid when:

| Criterion                 | Requirement |
| ------------------------- | ----------: |
| L2 error                  |     <= 5e-2 |
| L∞ error                  |     <= 5e-2 |
| Spatial convergence order |         2.0 |
| Refinement study          |        PASS |

The acceptance thresholds are selected based on the expected truncation error of the second-order finite-volume diffusion discretization.

The discretization error should scale as:

$$
E=O(h^2)
$$

The refinement study provides the primary verification of spatial accuracy by confirming that the observed convergence rate approaches the theoretical second-order behavior.

The case was validated using the mesh refinement sequence:

$$
N={16,32,64,128,256}
$$

The observed convergence rates were:

$$
p_{L2}=2.00071
$$

$$
p_{L\infty}=1.99881
$$

The final refined solution produced:

* Mesh: (256 $\times$ 256)
* Solver: CG

L2 error:

$$
L_2=6.40893\times10^{-5}
$$

L∞ error:

$$
L_{\infty}=1.19218\times10^{-4}
$$

Both accuracy and refinement criteria were satisfied.

The final error is significantly below the acceptance threshold, confirming the correct implementation of mixed Dirichlet, Neumann, and convective boundary conditions as well as second-order spatial discretization accuracy.

---

## Verification Parameters

Example configuration:

```json
{
  "extends": "../../base.json",

  "verificationCase": {
    "name": "Robin2D",
    "type": "manufactured",

    "mesh": {
      "type": "quad2D",
      "nx": 256,
      "ny": 256,
      "lx": 1.0,
      "ly": 1.0
    },

    "refinement": {
      "enabled": true,
      "levels": [16,32,64,128,256],
      "expected_order": 2.0
    },

    "physics": {
      "type": "heat",
      "k": 100.0,
      "volumetricSource": 400.0
    },

    "solver": {
      "method": "CG",
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
        "type": "Convective",
        "h": 10.0,
        "ambientTemperature": 93.0
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
      "csv": "Robin2D.csv",
      "summary": "Robin2D.json"
    }
  }
}
```
