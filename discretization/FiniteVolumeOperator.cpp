#include "discretization/FiniteVolumeOperator.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"

#include "physics/PhysicsModel.hpp"
#include "linear_system/LinearSystem.hpp"

#include <limits>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <algorithm>

// =========================================================
// Core assembly
// =========================================================

void FiniteVolumeOperator::assemble(
    const FluxAccumulator& flux,
    LinearSystem& sys) const
{
    sys.clear();

    // =========================================================
    // 1. DIFFUSION
    // =========================================================
    diffusion_.assemble(flux, sys);

    // =========================================================
    // 2. CONVECTION
    // =========================================================
    convection_.assemble(flux, sys);

    // =========================================================
    // 3. SOURCES + BCs
    // =========================================================

    for (std::size_t c = 0; c < flux.size(); ++c)
    {
        sys.addRHS(c, flux[c].Su);
        sys.addCoeff(c, c, -flux[c].Sp);
    }
}