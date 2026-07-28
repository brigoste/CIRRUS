# Verification Suite

This directory documents the manufactured solutions and benchmark problems used to verify
the numerical implementation of CIRRUS.

Each verification case is designed to isolate one aspect of the discretization or solver
and compares the numerical solution against an analytical solution.

## Current Verification Cases

| Case | Physics | Dimension | Purpose |
|------|----------|-----------|---------|
| Linear1D | Heat Diffusion | 1D | Exact linear diffusion solution |
| Quadratic1D | Heat Diffusion + Source | 1D | Verifies volumetric source implementation |
| Sinusoidal2D | Heat Diffusion | 2D | Verifies second-order diffusion accuracy |
| AdvectionDiffusion2D | Advection-Diffusion | 2D | Verifies coupled advection-diffusion discretization |

## Verification Metrics

Each case reports

- L2 Error
- L∞ Error
- Observed Order of Accuracy (when refinement is enabled)

The observed order is computed as

$$ p = \frac{ \log(E_h/E_{h/2}) }{ \log(h/(h/2) )}$$

where

- $(E_h)$ is the error on the coarse mesh
- $(E_{h/2})$ is the error on the refined mesh.

## Directory Structure

Each verification run produces

```
output/
└── validation/
    └── CaseName/
        ├── CaseName.csv
        ├── CaseName.json
        ├── CaseName_L0.csv
        ├── CaseName_L1.csv
        └── ...
```

The CSV files contain the numerical solution while the JSON files contain verification
metadata including error norms and convergence information.
