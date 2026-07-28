# 04_AdvectionDiffusion2D

## Purpose

This case verifies the implementation of the coupled advection-diffusion operator within the steady transport model. The problem represents two-dimensional transport through a square domain with constant diffusion, uniform advection velocity, and a manufactured sinusoidal solution.

Unlike the previous diffusion-only verification cases, this case activates the advection contribution. This provides verification of advection flux assembly, diffusion-advection coupling, source term integration, boundary condition enforcement, linear system assembly, and iterative solver performance.

This case verifies:

* steady diffusion operator implementation
* advection flux calculation
* diffusion-advection coupling
* manufactured source term application
* Dirichlet boundary condition enforcement
* linear system assembly
* iterative solver functionality

---

## Governing Equation

The steady advection-diffusion equation is:

$$
\rho(\mathbf{u}\cdot\nabla \phi)-\nabla\cdot(\gamma\nabla\phi)=S
$$

where:

* $(\rho)$: density
* $(\mathbf{u})$: advection velocity vector
* $(\phi)$: transported scalar variable
* $(\gamma)$: diffusion coefficient
* $(S)$: manufactured source term

Expanding the diffusion operator for constant diffusion coefficient:

$$
\rho(u_x\frac{\partial\phi}{\partial x}
+
u_y\frac{\partial\phi}{\partial y})
-
\gamma
\left(
\frac{\partial^2\phi}{\partial x^2}
+
\frac{\partial^2\phi}{\partial y^2}
\right)
=
S
$$

The advection term transports the scalar field through the domain, while the diffusion term smooths spatial gradients.

---

## Domain

The problem uses a two-dimensional computational domain.

The domain is defined as:

$$
0 \leq x \leq L_x
$$

$$
0 \leq y \leq L_y
$$

For this verification case:

| Parameter | Value |
| --------- | ----- |
| Domain length | 1.0 m |
| Domain height | 1.0 m |
| Number of cells | 40 x 40 |
| Mesh type | Uniform quad2D |

The refinement study uses:

| Level | Mesh |
| ----- | ---- |
| 0 | 40 x 40 |
| 1 | 80 x 80 |
| 2 | 160 x 160 |
| 3 | 320 x 320 |

The mesh is generated using uniform finite volumes with the scalar variable stored at cell centers.

---

## Boundary Conditions

Dirichlet boundary conditions are applied on all four boundaries.

The manufactured solution satisfies:

$$
\phi(x,y)=\sin(\pi x)\sin(\pi y)
$$

Since:

$$
\sin(0)=\sin(\pi)=0
$$

all boundaries have zero scalar value.

The boundary conditions are:

| Group | Boundary | Type | Value |
| ----- | -------- | ---- | ----- |
| 0 | Left | Dirichlet | 0.0 |
| 1 | Right | Dirichlet | 0.0 |
| 2 | Bottom | Dirichlet | 0.0 |
| 3 | Top | Dirichlet | 0.0 |

---

## Manufactured Solution

The analytical solution is selected as:

$$
\phi(x,y)=\sin(\pi x)\sin(\pi y)
$$

The first derivatives are:

$$
\frac{\partial\phi}{\partial x}
=
\pi\cos(\pi x)\sin(\pi y)
$$

and:

$$
\frac{\partial\phi}{\partial y}
=
\pi\sin(\pi x)\cos(\pi y)
$$

The Laplacian is:

$$
\nabla^2\phi
=
\frac{\partial^2\phi}{\partial x^2}
+
\frac{\partial^2\phi}{\partial y^2}
$$

which gives:

$$
\nabla^2\phi
=
-2\pi^2
\sin(\pi x)
\sin(\pi y)
$$

or:

$$
\nabla^2\phi=-2\pi^2\phi
$$

The manufactured source term is calculated from the governing equation:

$$
S=
\rho(u_x\phi_x+u_y\phi_y)
-
\gamma\nabla^2\phi
$$

This source term produces the analytical solution exactly in the continuous equation.

---

## Physical Parameters

The verification case uses:

$$
\gamma=1.0
$$

$$
\rho=1.0
$$

$$
u_x=1.0
$$

$$
u_y=0.5
$$

The advection velocity introduces transport in both coordinate directions.

---

## Source Term

The source term consists of both advection compensation and diffusion compensation.

The advection contribution is:

$$
S_A=
\rho
\left(
u_x\frac{\partial\phi}{\partial x}
+
u_y\frac{\partial\phi}{\partial y}
\right)
$$

The diffusion contribution is:

$$
S_D=-\gamma\nabla^2\phi
$$

The complete source term is:

$$
S=S_A+S_D
$$

The manufactured source is integrated over each finite volume during system assembly.

---

## Discretization Details

The AdvectionDiffusion2D verification case uses a cell-centered finite-volume discretization.

The governing equation is integrated over each control volume:

$$
\int_{V_P}
\rho(\mathbf{u}\cdot\nabla\phi)dV
-
\int_{V_P}
\nabla\cdot(\gamma\nabla\phi)dV
=
\int_{V_P}S dV
$$

Applying the divergence theorem to the diffusion term:

$$
-\sum_f
\gamma A_f
\frac{\partial\phi}{\partial n}
=
SV_P
$$

The resulting discrete equation has the form:

$$
a_P\phi_P
=
a_E\phi_E
+
a_W\phi_W
+
a_N\phi_N
+
a_S\phi_S
+
b_P
$$

where:

* diffusion contributions are assembled through face gradients
* advection contributions are assembled through transport fluxes
* $b_P$ contains the integrated source contribution

---

## Solver Configuration

The advection-diffusion system produces a non-symmetric matrix due to the advection contribution.

The system is solved using the BiCGSTAB iterative solver.

| Parameter | Value |
| --------- | ----- |
| Solver | BiCGSTAB |
| Tolerance | $1e^{-8}$ |
| Maximum iterations | 5000 |

BiCGSTAB is selected because the advection term removes the symmetric positive definite property required by Conjugate Gradient methods.

---

## Verification Procedure

The numerical solution is compared against the manufactured analytical solution.

The following error metrics are calculated.

### L2 Norm

$$
L_2=
\sqrt{
\frac{1}{N}
\sum_{i=1}^{N}
(\phi_i-\phi_i^{exact})^2
}
$$

### L∞ Norm

$$
L_\infty=
\max_i|\phi_i-\phi_i^{exact}|
$$

A mesh refinement study is performed to determine the observed convergence order.

---

## Expected Results

The current advection discretization is expected to produce approximately first-order spatial convergence when advection is active.

Although the diffusion operator is second-order accurate, the overall coupled advection-diffusion scheme is limited by the accuracy of the advection treatment.

The acceptance thresholds are:

| Metric | Threshold |
| ------ | --------- |
| $L_2$ | $1e^{-2}$ |
| $L_\infty$ | $2e^{-2}$ |

The refinement results are:

| Mesh | $L_2$ Error | $L_\infty$ Error |
| ---- | ----------- | ---------------- |
| 40 x 40 | 4.12406e-03 | 8.42672e-03 |
| 80 x 80 | 2.19280e-03 | 4.43381e-03 |
| 160 x 160 | 1.12948e-03 | 2.27240e-03 |
| 320 x 320 | 5.73059e-04 | 1.15010e-03 |

Observed convergence:

$$
p_{L2}=0.949
$$

$$
p_{L_\infty}=0.958
$$

The observed convergence is consistent with first-order accuracy.

The results satisfy the accuracy criteria and confirm that the advection-diffusion operator, source implementation, boundary conditions, and iterative solver are functioning correctly.

---

## Future Development Note

This verification case establishes a baseline for the current advection-diffusion implementation.

The present formulation exhibits approximately first-order spatial convergence when advection is active. This behavior is expected because the current advection discretization determines the overall accuracy of the coupled transport operator.

As higher-order advection schemes are incorporated into CIRRUS, this case should be revisited to measure improvements in transport accuracy and verify that the expected higher-order convergence is achieved.

---

## Components Verified

The AdvectionDiffusion2D case exercises the following components:

| Component | Tested |
| --------- | ------ |
| 2D mesh generation | ✓ |
| Cell-centered finite volume discretization | ✓ |
| Diffusion flux calculation | ✓ |
| Advection flux calculation | ✓ |
| Advection-diffusion coupling | ✓ |
| Source term integration | ✓ |
| Dirichlet boundary conditions | ✓ |
| Linear system assembly | ✓ |
| BiCGSTAB solver | ✓ |
| Analytical solution comparison | ✓ |
| Verification error calculation | ✓ |
| Mesh refinement study | ✓ |

---

## Input Configuration

The following configuration was used:

```json
"AdvectionDiffusion2D": {

    "mesh": {
        "type": "quad2D",
        "nx": 20,
        "ny": 20,
        "lx": 1.0,
        "ly": 1.0
    },

    "refinement": {
        "enabled": true,
        "levels": [40, 80, 160, 320],
        "expected_order": 1.0
    },

    "physics": {
        "type": "advection-diffusion",
        "gamma": 1.0,
        "rho": 1.0,
        "ux": 1.0,
        "uy": 0.5,
        "uz": 0.0
    },

    "solver": {
        "type": "BiCGSTAB",
        "tol": 1e-8,
        "max_iter": 5000
    },

    "boundary_conditions": [
        { "group": 0, "type": "Dirichlet", "value": 0.0 },
        { "group": 1, "type": "Dirichlet", "value": 0.0 },
        { "group": 2, "type": "Dirichlet", "value": 0.0 },
        { "group": 3, "type": "Dirichlet", "value": 0.0 }
    ],

    "params": {}
}
