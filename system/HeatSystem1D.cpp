#include "system/HeatSystem1D.hpp"

#include "discretization/FiniteVolumeOperator.hpp"

#include "Solver/TDMA.hpp"
#include "Solver/GaussSeidel.hpp"
#include "Solver/SOR.hpp"
#include "Solver/CG.hpp"

#include "physics/HeatEquationModel.hpp"

#include <stdexcept>

// -----------------------------------------
// Constructor
// -----------------------------------------
HeatSystem1D::HeatSystem1D(const MeshBase& mesh,
                           const HeatCase1D& problem)
    : mesh_(mesh),
      sys_(mesh.ncells()),   // FIX: was mesh.size()
      problem_(problem)
{}

// -----------------------------------------
// Assembly (NOW PURE FV)
// -----------------------------------------
void HeatSystem1D::assemble()
{
    HeatEquationModel model;
    model.k  = problem_.k;
    model.Su = problem_.Su;
    model.Sp = problem_.Sp;

    // =====================================================
    // SINGLE RESPONSIBILITY:
    // FV operator handles EVERYTHING (flux + BC + source)
    // =====================================================
    FiniteVolumeOperator::assemble(mesh_, model, sys_);
}

// -----------------------------------------
// Solve
// -----------------------------------------
std::vector<double> HeatSystem1D::solve(
    SolverMethod method,
    int max_iter,
    double tol,
    double omega)
{
    switch (method)
    {
        case SolverMethod::CG:
            return CG(sys_, max_iter, tol);

        case SolverMethod::GS:
            return GaussSeidel(sys_, mesh_, max_iter, tol, false);

        case SolverMethod::SOR:
            return SOR(sys_, mesh_, max_iter, tol, omega, false);

        case SolverMethod::TDMA:
            return TDMA(sys_, mesh_.ncells(), false);  // FIX

        default:
            throw std::runtime_error("Unknown solver method");
    }
}

// -----------------------------------------
// Interface
// -----------------------------------------
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
    return sys_.size();
}

void HeatSystem1D::applyDirichlet(const BoundaryConfig& bc,
                                  const BoundaryContext& ctx)
{
    int i = ctx.owner;

    sys_.clearRow(i);
    sys_.addDiag(i, 1.0);
    sys_.setRHS(i, bc.value);
}
void HeatSystem1D::applyNeumann(const BoundaryConfig& bc,
                                const BoundaryContext& ctx)
{
    int i = ctx.owner;

    sys_.addRHS(i, bc.flux * ctx.area);
}
void HeatSystem1D::applyConvective(const BoundaryConfig& bc,
                                   const BoundaryContext& ctx)
{
    int i = ctx.owner;

    sys_.addDiag(i, bc.h * ctx.area);
    sys_.addRHS(i, bc.h * bc.Tinf * ctx.area);
}
