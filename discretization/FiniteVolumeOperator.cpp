#include "discretization/FiniteVolumeOperator.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/Face.hpp"

#include "physics/HeatEquationModel.hpp"
#include "linear_system/LinearSystem.hpp"

#include <limits>
#include <stdexcept>
#include <iostream>

// =========================================================
// Core assembly
// =========================================================

void FiniteVolumeOperator::assemble(
    // const MeshBase& mesh,
    // const HeatEquationModel& model,
    const FluxAccumulator& flux,
    LinearSystem& sys)
{
    sys.clear();

    // =========================================================
    // 1. DIFFUSION
    // =========================================================
    for (const auto& f : flux.diffusion())
    {
        const auto P = f.P;
        const auto N = f.N;
        const double D = f.D;

        sys.addCoeff(P, P,  D);
        sys.addCoeff(P, N, -D);
        sys.addCoeff(N, N,  D);
        sys.addCoeff(N, P, -D);
    }

    // =========================================================
    // 2. CONVECTION (verify this later if unstable)
    // =========================================================
    for (const auto& f : flux.convection())
    {
        const auto P = f.P;
        const auto N = f.N;
        const double F = f.F;

        const double Fp = std::max(F, 0.0);
        const double Fm = std::max(-F, 0.0);

        sys.addCoeff(P, P,  Fp);
        sys.addCoeff(P, N, -Fp);

        sys.addCoeff(N, N,  Fm);
        sys.addCoeff(N, P, -Fm);
    }

    // =========================================================
    // 3. SOURCES + BCs (ONLY place they should exist now)
    // =========================================================
    for (std::size_t c = 0; c < flux.size(); ++c)
    {
        const auto& cell = flux[c];

        sys.addRHS(c, cell.Su);
        sys.addCoeff(c, c, -cell.Sp);
    }

    // std::cout << "[DEBUG] assembling system, size = " << sys.size() << "\n";
}
