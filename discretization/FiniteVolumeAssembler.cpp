#include "discretization/FiniteVolumeAssembler.hpp"

// =========================================================
// Core assembly
// =========================================================

void FiniteVolumeAssembler::assemble(
    const MeshBase& mesh,
    FluxAccumulator& flux,
    const ScalarField& field,
    const VectorField& gradient,
    EquationSystem& sys) const
{
    // =========================================================
    // 1. DIFFUSION
    // =========================================================
    diffusion_.assemble(mesh, flux);

    // =========================================================
    // 2. CONVECTION
    // =========================================================
    convection_.assemble(mesh, flux, field, gradient);

    for (const auto& contribution : flux.matrixContributions())
    {
        sys.addCoeff(
            contribution.row,
            contribution.column,
            contribution.coefficient
        );
    }

    // =========================================================
    // 3. SOURCES + BCs
    // =========================================================

    for (const auto& contribution : flux.sourceContributions())
    {
        sys.addRHS(
            contribution.cell,
            contribution.Su
        );

        sys.addCoeff(
            contribution.cell,
            contribution.cell,
            -contribution.Sp
        );
    }
}
