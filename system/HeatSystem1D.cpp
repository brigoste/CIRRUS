#include "system/HeatSystem1D.hpp"

#include "discretization/FiniteVolumeOperator.hpp"
#include "Solver/TDMA.hpp"
#include "Solver/GaussSeidel.hpp"
#include "Solver/SOR.hpp"
#include "Solver/CG.hpp"
#include "Solver/SolverMethod.hpp"

#include "discretization/FluxBuilder.hpp"

#include <stdexcept>
#include <iostream>

// =========================================================
// Constructor
// =========================================================

HeatSystem1D::HeatSystem1D(
    const MeshBase& mesh,
    const BoundaryPatchSystem& boundary)
    : mesh_(mesh),
      sys_(mesh.ncells()),
      boundary_(boundary),
      flux_(mesh.ncells())
{}

// =========================================================
// Assembly
// =========================================================

void HeatSystem1D::assemble(const HeatEquationModel& model)
{
    flux_.reset();

    FluxBuilder::buildFlux(mesh_, model, boundary_, flux_);

    sys_.clear();   // ✔ REQUIRED for accumulation-based system

    FiniteVolumeOperator::assemble(
        // mesh_,
        // model,
        flux_,
        sys_);
}

// =========================================================
// Solve
// =========================================================

std::vector<double> HeatSystem1D::solve(
    solver::Method method,
    int max_iter,
    double tol,
    double omega)
{
    // std::cout << "Solver: " << to_string(method) << "\n";

    switch (method)
    {
        case solver::Method::CG:
            return CG(sys_, max_iter, tol);

        case solver::Method::GS:
            return GaussSeidel(sys_, max_iter, tol);

        case solver::Method::SOR:
            return SOR(sys_, max_iter, tol, omega);

        case solver::Method::TDMA:
            return TDMA(sys_);

        default:
            throw std::runtime_error("Unknown solver method");
    }
}

// =========================================================
// Interface
// =========================================================

const MeshBase& HeatSystem1D::mesh() const
{
    return mesh_;
}

const LinearSystem& HeatSystem1D::system() const
{
    return sys_;
}

int HeatSystem1D::size() const
{
    return static_cast<int>(sys_.size());
}
