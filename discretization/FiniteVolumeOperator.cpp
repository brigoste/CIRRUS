#include "discretization/FiniteVolumeOperator.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"

#include "physics/PhysicsModel.hpp"
#include "linear_system/LinearSystem.hpp"

#include <limits>
#include <stdexcept>
#include <iostream>
#include <numeric>

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
    // std::cout << "[DEBUG] assembling system\n";

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
    // 3. SOURCES + BCs
    // =========================================================
    for (std::size_t c = 0; c < flux.size(); ++c)
    {
        sys.addRHS(c, flux[c].Su);
        sys.addCoeff(c, c, -flux[c].Sp);
    }
    // std::cout << "[DEBUG] assembling system, size = " << sys.size() << "\n";
    // for (unsigned int i = 0; i < sys.size(); ++i)
    // {
    //     auto row = sys.row(i);
    //     std::cout << "row " << i
    //             << " diag = " << row[i]
    //             << " rhs = " << sys.rhs(i) << "\n";
    // }
}
