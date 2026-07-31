# Convective1D Verification Case

## Overview

The `Convective1D` verification case validates the implementation of a convective boundary condition for the 1D steady heat equation.

The case uses a manufactured quadratic temperature field with:

* a Dirichlet boundary condition at the left boundary
* a convective heat transfer boundary condition at the right boundary
* a constant thermal conductivity

The convective boundary condition is mathematically a Robin boundary condition of the form:

$$
-k\frac{dT}{dn}=h(T_\infty-T)
$$

This case verifies:

* correct application of convective boundary conditions
* correct incorporation of the heat transfer coefficient
* correct coupling between the solution temperature and ambient temperature
* second-order spatial accuracy of the diffusion discretization

---

## Governing Equation

The steady 1D heat equation is:

$$
-k\frac{d^2T}{dx^2}=q'''
$$

where:

* $(k)$ is the thermal conductivity
* $(q''')$ is the volumetric heat source
* $(T)$ is the temperature field

---

## Boundary Conditions

The manufactured solution uses the following boundary conditions:

### Left Boundary (Dirichlet)

At $x=0$:

$$
T(0)=T_L
$$

where $T_L$ is specified by the input configuration.

---

### Right Boundary (Convective)

At $x=L$:

$$
-k\frac{dT}{dx}(L)=h(T_\infty-T(L))
$$

where:

* $h$ is the convective heat transfer coefficient
* $T_\infty$ is the ambient fluid temperature

In CIRRUS, this boundary condition is incorporated into the finite-volume source coefficients:

$$
S_u=hA_fT_\infty
$$

$$
S_p=-hA_f
$$

where:

* $A_f$ is the boundary face area
* $S_u$ is the source contribution
* $S_p$ is the linearized sink coefficient

This is equivalent to applying a Robin boundary condition while maintaining the linear system form required by the finite-volume solver.

---

## Manufactured Solution

The manufactured temperature field is:

$$
T(x)=T_0+a x+b x^2
$$

The corresponding derivatives are:

$$
\frac{dT}{dx}=a+2bx
$$

and:

$$
\frac{d^2T}{dx^2}=2b
$$

The volumetric source term is computed from:

$$
q'''=-k\frac{d^2T}{dx^2}
$$

The convective boundary parameters are selected such that:

$$
-k\frac{dT}{dx}(L)=h(T_\infty-T(L))
$$

ensuring that the manufactured solution satisfies the imposed boundary condition.

---

## Verification Results

The following configuration was used to validate the implementation of the `Convective1D` manufactured solution.

### Acceptance Criteria

A verification run is considered successful when both the accuracy and refinement requirements are satisfied.

| Metric | Requirement |
| --- | ---: |
| L₂ Error | ≤ 5×10⁻² |
| L∞ Error | ≤ 5×10⁻² |
| Observed Spatial Order | ≈ 2.0 |

The primary verification metric for this case is the observed convergence order. The refinement study confirms that the convective boundary treatment maintains the expected second-order spatial accuracy.

---

## Mesh Refinement Results

The following refinement study was performed using the manufactured solution.

| Mesh | Cell Size ($h$) | L₂ Error | L∞ Error |
| --- | ---: | ---: | ---: |
| 25 × 1 | 0.0400 | 4.000×10⁻⁴ | 4.000×10⁻⁴ |
| 50 × 1 | 0.0200 | 1.000×10⁻⁴ | 1.000×10⁻⁴ |
| 100 × 1 | 0.0100 | 2.500×10⁻⁵ | 2.500×10⁻⁵ |
| 200 × 1 | 0.0050 | 6.250×10⁻⁶ | 6.250×10⁻⁶ |

---

## Observed Convergence

| Quantity | Value |
| --- | ---: |
| Observed L₂ Order | 2.000 |
| Observed L∞ Order | 2.000 |
| Expected Order | 2.000 |

The refinement study demonstrates second-order spatial convergence of the finite-volume diffusion discretization.

The convective boundary condition implementation, using the Robin formulation:

$$
-k\frac{dT}{dn}=h(T_\infty-T)
$$

is therefore verified.

---

## JSON Configuration

```json
{
  "extends": "../base.json",

  "verificationCase": {
    "name": "Convective1D",
    "type": "manufactured",

    "mesh": {
      "type": "line1D",
      "nx": 200,
      "lx": 1.0
    },

    "refinement": {
      "enabled": true,
      "levels": [25,50,100,200],
      "expected_order": 2.0
    },

    "physics": {
      "type": "heat",
      "k": 100.0,
      "volumetricSource": -200.0
    },

    "solver": {
      "type": "TDMA",
      "tol": 1e-8,
      "max_iter": 5000
    },

    "boundary_conditions": [
      {
        "group": 0,
        "type": "Dirichlet",
        "value": 1.0
      },
      {
        "group": 1,
        "type": "Convective",
        "h": 10.0,
        "ambientTemperature": 84.0
      }
    ],

    "norms": {
      "l2": true,
      "linf": true
    },

    "output": {
      "csv": "Convective1D.csv",
      "summary": "Convective1D.json"
    }
  }
}
