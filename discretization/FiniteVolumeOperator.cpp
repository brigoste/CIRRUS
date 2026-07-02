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

// static volatile int fv_loaded = [](){
//     std::cout << ">>> FV FILE LOADED <<<\n";
//     return 0;
// }();

// =========================================================
// Core assembly
// =========================================================

void FiniteVolumeOperator::assemble(
    const FluxAccumulator& flux,
    LinearSystem& sys)
{
    sys.clear();

    // =========================================================
    // 1. DIFFUSION
    // =========================================================
    std::cout << "diffusion faces = " << flux.diffusion().size() << "\n";

    for (const auto& f : flux.diffusion())
    {
        const auto P = f.P;
        const auto N = f.N;
        const double D = f.D;

        if (f.type == FaceType::Interior)
        {
            sys.addCoeff(P, P,  D);
            sys.addCoeff(P, N, -D);

            sys.addCoeff(N, N,  D);
            sys.addCoeff(N, P, -D);
        }
    }

    // =========================================================
    // 2. CONVECTION (1st-order upwind)
    // =========================================================
    for (const auto& f : flux.convection())
    {
        const auto P = f.P;
        const auto N = f.N;
        const double F = f.F;

        const double Fp = std::max(F,  0.0);   // P -> N
        const double Fn = std::max(-F, 0.0);   // N -> P

        sys.addCoeff(P, P,  Fp);
        sys.addCoeff(P, N, -Fn);

        sys.addCoeff(N, N,  Fn);
        sys.addCoeff(N, P, -Fp);
    }

    // =========================================================
    // 3. SOURCES + BCs
    // =========================================================

    for (std::size_t c = 0; c < flux.size(); ++c)
    {
        sys.addRHS(c, flux[c].Su);
        sys.addCoeff(c, c, -flux[c].Sp);
    }
}
