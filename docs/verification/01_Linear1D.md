# 01_Linear1D

## Purpose

This case explores the solver's capability with simple conduction in a uniform rod of constant diameter. The problem is treated as one-dimenionsal conduction. This case gives the priliminary test for the solver's linear system assembly, heat conduction model, and overall solution pipeline. It acts as the first verification test for the complete simulation workflow.

This case verifies the implementation of the steady diffusion operator, Dirichlet boundary condition enforcement, linear system assembly, and baseline solver functionality.

---

## Governing Equation

Describe the continuous equation being solved.

The governing equation for steady, one-dimensional heat diffusion is as follows:


$$-\frac{d}{dx}(k\frac{dT}{dx}) = 0$$


The variables used in this form represent the following:

- $( k )$ : thermal conductivity ($W/m·^\circ C$)
- $( x )$ : distance along rod (m)
- $( T )$ : temperature along rod ( $^\circ C$)

This problem assumes uniform, non-zero thermal conductivity allowing the problem to be simplified to the following forms: 

$$-k\frac{d^2 T}{dx^2} = 0$$


$$\frac{d^2 T}{dx^2} = 0$$

---

## Domain

The problem uses a 1D computational domain. The problem is defined along the x-axis starting at x = 0 to x = L, where L is the length of the rod. In our example, the rod length is 1.5 meters.

The mesh is generated using evenly spaced cells and half-cells at the boundaries. The input for mesh generation is the total length of the rod and the number of cells to generate. 

---

## Boundary Conditions

Only Dirichlet boundary conditions were imposed on the system at x=0 and x=L. The only physical boundaries are located at x=0 and x=L. Dirichlet conditions are applied at both boundaries. No additional source terms or flux boundary conditions are present. The two Dirichlet conditions applied were of the following form:

$$ T(0) = T_L = 300^\circ C $$

$$ T(1.5) = T_R = 400^\circ C $$

The boundary conditions were mapped to the solution using boundary groups 0 and 1.

Example:

| Group | Boundary | Type | Value |
|---|---|---|---|
| 0 | Left | Dirichlet | $300^\circ C$ |
| 1 | Right | Dirichlet | $400^\circ C$ |

---
## Source Term

The problem setups included no additional source terms. 

---

## Manufactured Solution

The analytical solution to this boundary-value problem is a linear temperature distribution between the two boundaries:

$$\frac{d^2T}{dx^2} = 0$$

Integrating:

$$\frac{dT}{dx} = c_1$$

Integrating again:

$$T(x) = (c_1x +c_2)$$

We can evaulate the constant by plugging in our boundary conditionsl

$$\bigg[T(x) = (c_1x +c_2)\bigg]\bigg|_{x=0}^L$$

$$ c_2 = T_L $$

$$ c_1 = \frac{T_R - T_L}{L} $$

$$ T(x) = T_L + \frac{T_R - T_L}{L}x$$

This analytical solution is used as the reference solution for error evaluation.

Substituting in our values of $T_R$, $T_L$ and $L$ we get

$$ T(x) = 300 + x\frac{400 - 300}{1.5}$$

$$ T(x) = 300 + x\frac{100}{1.5} $$

$$ T(x) = 300 + 66.667x $$

---

## Discretization Details

The Linear1D verification case uses a cell-centered finite-volume discretization of the steady one-dimensional heat equation.

The governing equation is integrated over each control volume:

$$ -\int_{V_P} \frac{d}{dx}\left(k\frac{dT}{dx}\right)dV = 0 $$

Applying the divergence theorem converts the volume integral into a balance of diffusive fluxes through the cell faces:

$$ -\sum_f k A_f (\nabla T \cdot \vec{n})_f = 0 $$

The form of the divergence can be written for 1D in the following form:

$$ (\nabla T \cdot \vec{n})_f \rightarrow \frac{T_N - T_P}{d_{PN}} $$

### Spatial Discretization

The domain is divided into uniform finite volumes. Temperature is stored at the center of each control volume:

$$ T_P = T(x_P) $$

where $P$ denotes the cell center.

### Flux Approximation

The diffusive flux between neighboring cells is approximated using a second-order central difference:

$$ \frac{dT}{dx}\bigg|_f \approx \frac{T_N-T_P}{d_{PN}} $$

where:

- $T_P$ is the temperature of the current cell
- $T_N$ is the neighboring cell temperature
- $d_{PN}$ is the distance between cell centers

The resulting face flux is:

$$ F_f = -kA_f \frac{T_N-T_P}{d_{PN}} $$

The value of $d_{PN}$ works for non-uniformilly sized cells as well as uniform spacing. Because the mesh is uniform and orthogonal, this approximation is formally second-order accurate.

### Integration Method

The discretization is:

- cell-centered
- conservative
- finite-volume based

The discrete equation for each control volume is assembled into a linear system:

$$ a_P T_P = a_E T_E + a_W T_W $$

where the coefficients represent the diffusive contributions from neighboring faces.

The resulting system is solved using the configured linear solver.

---

## Solver Configuration

The solver used for this example is the Tridiagnoal Matrix Algorithm, or TDMA. TDMA was selected because the one-dimensional diffusion discretization produces a tridiagonal coefficient matrix, allowing an efficient direct solution. TDMA is a direct solver for tridiagonal systems and therefore does not use iterative convergence criteria.

| Parameter | Value |
|---|---|
| Solver | TDMA |
| Tolerance | NA |
| Maximum iterations | NA |


---

## Verification Procedure

The results of the test were compared aginst the exact solution. In order to determine how closely the solution compares to the exact solution, the following error metrics are calculated:

### L2 Norm

$$ L_2 = \sqrt{ \frac{1}{N} \Sigma^N_{i=1}(T_i-T_i^{exact})^2} $$

### L∞ Norm

$$ L_\infty = \max_i |T_i-T_i^{exact}| $$


---

## Expected Results

The criteria for accuracy were applied to the calculated norms. For this tests, a tolerance of $1e-10$ was required for both $L_2$ and $L_\infty$.

| Metric | Expected |
|---|---|
| Accuracy | PASS |


In our case, the value of $L_2$ and $L_{\infty}$ are shown below.

| Error | Value |
|---|---|
| $L_2$ | 3.008e-13 |
| $L_\infty$ | 3.97904e-13 |

The computed error is approximately two orders of magnitude below the verification threshold and approaches the numerical limits expected from double-precision finit-volume implmentation. These results indicate that the diffusion operator, boundary condition implementation, and linear solver are correctly reproducing the expected analytical solution.

---
## Components Verified
The Linear1D case exercises the following components:
| Component | Tested|
|---|---|
| Mesh generation | ✓ |
| Cell-centered finite volume discretization | ✓ |
| Diffusion flux calculation | ✓ |
| Dirichlet boundary conditions | ✓ |
| Linear system assembly | ✓ |
| TDMA solver | ✓ |
| Analytical solution comparison | ✓ |
| Verification error calculation | ✓ |

---

## Input Configuration

The following configuration was used to execute the verification case:

```json
{
  "extends": "../base.json",

  "verificationCase": {
    "name": "Linear1D",
    "type": "manufactured",

    "plot_enabled": true,

    "mesh": {
      "type": "line1D",
      "nx": 20,
      "lx": 1.0
    },

    "refinement": {
      "enabled": false,
      "levels": [10, 20, 40, 80, 160],
      "expected_order": 2.0
    },

    "physics": {
      "type": "heat",
      "k": 100.0,
      "volumetricSource": 0.0
    },

    "solver": {
      "method": "TDMA",
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
        "type": "Dirichlet",
        "value": 200.0
      }
    ],

    "norms": {
      "l2": true,
      "linf": true
    },

    "output": {
      "csv": "Linear1D.csv",
      "summary": "Linear1D.json"
    }
  }
}
