# Neumann1D Verification Case

## Overview

The `Neumann1D` verification case validates the implementation of a mixed Dirichlet-Neumann boundary condition for the 1D steady heat equation.

The case uses a manufactured quadratic temperature field with:
- a Dirichlet boundary condition at the left boundary
- a Neumann heat flux boundary condition at the right boundary
- a constant volumetric source term

This case verifies:
- correct application of Neumann boundary fluxes
- correct incorporation of volumetric source terms
- second-order spatial accuracy of the diffusion discretization

---

## Governing Equation

The steady 1D heat equation is:

\[
-k\frac{d^2T}{dx^2}=q'''
\]

where:

- \(k\) is the thermal conductivity
- \(q'''\) is the volumetric heat source
- \(T\) is the temperature field

---

## Boundary Conditions

The manufactured solution uses the following boundary conditions:

### Left Boundary (Dirichlet)

At \(x=0\):

\[
T(0)=T_L
\]

where \(T_L\) is specified by the input configuration.

### Right Boundary (Neumann)

At \(x=L\):

\[
k\frac{dT}{dx}(L)=q_R
\]

where \(q_R\) is the specified boundary heat flux.

In CIRRUS, Neumann boundary conditions are applied through a source contribution:

\[
S_u=-q_R A_f
\]

where \(A_f\) is the boundary face area.

---

## Manufactured Solution

The manufactured temperature field is:

\[
T(x)=T_L+
\left(\frac{-q_R+q'''L}{k}\right)x
-\frac{q'''}{2k}x^2
\]

The corresponding second derivative is:

\[
\frac{d^2T}{dx^2}=-\frac{q'''}{k}
\]

which satisfies:

\[
-k\frac{d^2T}{dx^2}=q'''
\]

---

## Verification Parameters

Example configuration:

```json
{
  "extends": "../base.json",

  "verificationCase": {
    "name": "Neumann1D",
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
      "volumetricSource": -10000.0
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
        "value": 100.0
      },
      {
        "group": 1,
        "type": "Neumann",
        "flux": -10000.0
      }
    ],

    "norms": {
      "l2": true,
      "linf": true
    },

    "output": {
      "csv": "Neumann1D.csv",
      "summary": "Neumann1D.json"
    }
  }
}
