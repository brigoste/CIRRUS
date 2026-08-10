#include "discretization/FiniteVolumeAssembler.hpp"

// =========================================================
// Core assembly
// =========================================================

void FiniteVolumeAssembler::assemble(
    const MeshBase& mesh,
    const FluxAccumulator& flux,
    EquationSystem& sys) const
{
    // =========================================================
    // 1. DIFFUSION
    // =========================================================
    diffusion_.assemble(mesh, flux, sys);

    // =========================================================
    // 2. CONVECTION
    // =========================================================
    convection_.assemble(mesh, flux, sys);

    // =========================================================
    // 3. SOURCES + BCs
    // =========================================================

    for (std::size_t c = 0; c < flux.size(); ++c)
    {
        sys.addRHS(c, flux[c].Su);
        sys.addCoeff(c, c, -flux[c].Sp);
    }
}