# 02_Quadratic1D

## Purpose

This case verifies the implementation of volumetric source terms within the steady heat conduction model. The problem represents one-dimensional conduction through a uniform rod with constant thermal conductivity and a distributed internal heat generation term.

Unlike the Linear1D case, where the solution is linear and the diffusion operator can reproduce the analytical solution to machine precision, this case produces a quadratic temperature distribution. This provides a verification of source term integration, diffusion-source coupling, linear system assembly, and solver accuracy.

This case verifies:

* steady diffusion operator implementation
* volumetric source term application
* Dirichlet boundary condition enforcement
* finite-volume source integration
* linear system assembly
* iterative solver functionality

---

## Governing Equation

The governing equation for steady one-dimensional heat conduction with volumetric heat generation is:

$$ -\frac{d}{dx}\left(k\frac{dT}{dx}\right)=q''' $$

The variables represent:

* $(k)$: thermal conductivity (W/(m $\cdot$ K))
* $(x)$: distance along the rod (m)
* $(T)$: temperature $(^\circ C)$
* $(q''')$: volumetric heat generation source term $(W/m^3)$

Assuming constant thermal conductivity, the equation simplifies to:

$$ -k\frac{d^2T}{dx^2}=q''' $$

or:

$$ \frac{d^2T}{dx^2}=-\frac{q'''}{k} $$

A positive volumetric source represents internal heat generation. Under this sign convention, the source introduces negative curvature into the temperature field, causing the solution profile to become concave downward.

---

## Domain

The problem uses a one-dimensional computational domain along the x-axis.

The domain is defined as:

$$ 0 \leq x \leq L $$

For this verification case:

| Parameter       | Value          |
| --------------- | -------------- |
| Domain length   | 1.0 m          |
| Number of cells | 20             |
| Mesh type       | Uniform line1D |

The mesh is generated using evenly spaced finite volumes with temperature stored at cell centers.

---

## Boundary Conditions

Dirichlet boundary conditions are applied at both physical boundaries.

The left and right boundary temperatures are:

$$ T(0)=T_L=100^\circ C $$

$$ T(L)=T_R=200^\circ C $$

The boundary conditions are assigned through boundary groups:

| Group | Boundary | Type      | Value         |
| ----- | -------- | --------- | ------------- |
| 0     | Left     | Dirichlet | $100^\circ C$ |
| 1     | Right    | Dirichlet | $200^\circ C$ |

No flux boundary conditions are applied.

---

## Source Term

This case includes a constant volumetric heat generation term:

$$ q'''= 20000 \frac{W}{m^3} $$

The source term is included as a volumetric contribution to each finite volume:

$$ b_P=q'''V_P $$

where:

* $(b_P)$ is the integrated source contribution
* $(V_P)$ is the cell volume

The finite-volume formulation integrates the source over the control volume, resulting in an extensive source contribution rather than a pointwise value. The manufactured solution uses the same source term to create the expected analytical solution.

---

## Manufactured Solution

Starting from:

$$ \frac{d^2T}{dx^2}=-\frac{q'''}{k} $$

Integrating once:

$$ \frac{dT}{dx} = -\frac{q'''}{k}x+c_1 $$

Integrating again:

$$ T(x)  = -\frac{q'''}{2k}x^2+c_1x+c_2 $$

The constants are determined from the boundary conditions.

At (x=0):

$$ T(0)=T_L $$

therefore:

$$ c_2=T_L $$

At (x=L):

$$ T_R = -\frac{q'''}{2k}L^2+c_1L+T_L $$

Solving for (c_1):

$$ c_1= \frac{T_R-T_L}{L} + \frac{q'''L}{2k} $$

The final manufactured solution is:

$$ T(x)= T_L+ (\frac{T_R-T_L}{L}+\frac{q'''L}{2k})x - \frac{q'''} {2k}x^2 $$

This expression is identical to the analytical solution implemented in Quadratic1D::exact().

For this verification case:

$$ T_L=100,\quad T_R=200,\quad L=1.0,\quad k=100,\quad q'''=20000 $$

giving:

$$ T(x) = 100 + \bigg(\frac{200-100}{1} + \frac{20000(1)}{2(100)}  \bigg)x - \frac{20000}{2(100)}x^2 $$

$$ T(x)=100+200x-100x^2 $$

This analytical solution is used as the reference solution for error evaluation.

---

## Discretization Details

The Quadratic1D verification case uses a cell-centered finite-volume discretization of the steady heat equation with volumetric source terms.

The governing equation is integrated over each control volume:

$$ -\int_{V_P} \frac{d}{dx} \bigg(k\frac{dT}{dx}\bigg)dV =\int_{V_P} q'''dV $$

Applying the divergence theorem gives:

$$ -\sum_f kA_f \frac{dT}{dx}\bigg|_f = q'''V_P $$

### Spatial Discretization

The domain is divided into uniform finite volumes. Temperature is stored at cell centers:

$$ T_P=T(x_P) $$

where (P) represents the cell center.

### Flux Approximation

Diffusive fluxes are approximated using a second-order central difference:

$$ \frac{dT}{dx}\bigg|_f \approx \frac{T_N-T_P}{d_{PN}} $$

where:

* $(T_P)$: current cell temperature
* $(T_N)$: neighboring cell temperature
* $(d_{PN})$: distance between cell centers

The resulting diffusive face flux is:

$$ F_f= -kA_f \frac{T_N-T_P}{d_{PN}} $$

### Source Integration

The volumetric source is integrated over each control volume:

$$ b_P=q'''V_P $$

This contribution is added to the linear system during assembly.

The resulting finite-volume equation is:

$$ a_PT_P=a_ET_E+a_WT_W+b_P $$

where (b_P) contains the source contribution. 

---

## Solver Configuration

For this steady diffusion problem with Dirichlet boundary conditions, the assembled system produces a symmetric positive definite matrix and is solved using the Conjugate Gradient solver.

| Parameter          | Value     |
| ------------------ | --------- |
| Solver             | CG        |
| Tolerance          | $1e^{-8}$ |
| Maximum iterations | 5000      |

CG convergence depends on the conditioning of the assembled matrix and the selected convergence tolerance.

---

## Verification Procedure

The numerical solution is compared against the manufactured analytical solution.

The following error metrics are calculated.

### L2 Norm

$$ L_2= \sqrt{ \frac{1}{N} \sum_{i=1}^{N} (T_i-T_i^{exact})^2 } $$

### L∞ Norm

$$ L_\infty= \max_i |T_i-T_i^{exact}| $$

---

## Expected Results

Because the manufactured solution is quadratic, the finite-volume discretization is expected to have second-order spatial accuracy.

For this coarse 20-cell mesh, the expected error is larger than Linear1D. Unlike Linear1D, the quadratic manufactured solution contains curvature. The cell-centered finite-volume approximation is second-order accurate but does not exactly reproduce quadratic fields on a coarse mesh.

The acceptance threshold for this case is:

| Metric     | Threshold |
| ---------- | --------- |
| $L_2$      | 7e-02 |
| $L_\infty$ | 7e-02 |

The computed errors are:

| Error      | Value     |
| ---------- | --------- |
| $L_2$      | 6.250e-02 |
| $L_\infty$ | 6.250e-02 |

The results satisfy the verification criteria and confirm that the diffusion operator, volumetric source implementation, and boundary condition enforcement are functioning correctly.

---

## Components Verified

The Quadratic1D case exercises the following components:

| Component                                  | Tested |
| ------------------------------------------ | ------ |
| Mesh generation                            | ✓      |
| Cell-centered finite volume discretization | ✓      |
| Diffusion flux calculation                 | ✓      |
| Volumetric source integration              | ✓      |
| Dirichlet boundary conditions              | ✓      |
| Linear system assembly                     | ✓      |
| CG solver                                  | ✓      |
| Analytical solution comparison             | ✓      |
| Verification error calculation             | ✓      |

---

## Input Configuration

The following configuration was used:

```json
"Quadratic1D": {

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
        "volumetricSource": 20000.0
    },

    "solver": {
        "type": "CG",
        "tol": 1e-8,
        "max_iter": 5000
    },

    "boundary_conditions": [
        { "group": 0, "type": "Dirichlet", "value": 100.0 },
        { "group": 1, "type": "Dirichlet", "value": 200.0 }
    ],

    "params": {}
}
```
