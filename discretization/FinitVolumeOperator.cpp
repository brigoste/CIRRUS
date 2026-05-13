#include "discretization/FiniteVolumeOperator.hpp"

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "physics/HeatEquationModel.hpp"

#include <vector>
#include <stdexcept>

// ------------------------------------------------------------
// Finite Volume Assembly (general diffusion + source/reaction)
// ------------------------------------------------------------
void FiniteVolumeOperator::assemble(
    const MeshBase& mesh,
    const HeatEquationModel& model,
    LinearSystem& sys)
{
    const int N = mesh.size();

    sys.resize(N);

    for (int i = 0; i < N; ++i)
    {
        double V = mesh.volume(i);

        double aP = 0.0;
        double bP = 0.0;

        // -----------------------------
        // Source terms (constant model)
        // -----------------------------
        double Su = model.Su;
        double Sp = model.Sp;

        bP += Su * V;
        aP += -Sp * V;

        // -----------------------------
        // Diffusion (neighbor stencil)
        // -----------------------------
        const auto& nbrs = mesh.neighbors(i);

        for (int j : nbrs)
        {
            if (j < 0 || j == i)
                continue;

            double dij = mesh.distance(i, j);

            if (dij <= 0.0)
                throw std::runtime_error("Invalid mesh distance");

            // 1D placeholder: face area = 1
            double Aij = mesh.faceArea(i, (j < i) ? NeighborDir::W : NeighborDir::E);

            double D = model.k * Aij / dij;

            sys.addCoeff(i, j, -D);
            aP += D;
        }

        // -----------------------------
        // diagonal + RHS
        // -----------------------------
        sys.addDiag(i, aP);
        sys.setRHS(i, bP);
    }
}
