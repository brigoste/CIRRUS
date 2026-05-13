#include "system/HeatSystem1D.hpp"

#include "discretization/FiniteVolumeOperator.hpp"

#include "Solver/TDMA.hpp"
#include "Solver/GaussSeidel.hpp"
#include "Solver/SOR.hpp"
#include "Solver/CG.hpp"

#include "bc/BoundaryConditionDescriptor.hpp"
#include "physics/HeatEquationModel.hpp"

#include <stdexcept>

// -----------------------------------------
// Constructor
// -----------------------------------------
HeatSystem1D::HeatSystem1D(const MeshBase& mesh,
                           const HeatCase1D& problem)
    : mesh_(mesh),
      sys_(mesh.size()),
      problem_(problem)
{}

// -----------------------------------------
// BC application
// -----------------------------------------
void HeatSystem1D::applyDirichlet(const BoundaryConditionDescriptor& bc,
                                  const BoundaryContext& ctx)
{
    int i = ctx.owner;

    sys_.clearRow(i);
    sys_.addDiag(i, 1.0);
    sys_.setRHS(i, bc.value);
}

void HeatSystem1D::applyNeumann(const BoundaryConditionDescriptor& bc,
                                const BoundaryContext& ctx)
{
    int i = ctx.owner;
    sys_.addRHS(i, bc.flux * ctx.area);
}

void HeatSystem1D::applyConvective(const BoundaryConditionDescriptor& bc,
                                   const BoundaryContext& ctx)
{
    int i = ctx.owner;  //Should this be neighbor?

    sys_.addDiag(i, bc.h * ctx.area);
    sys_.addRHS(i, bc.h * bc.Tinf * ctx.area);
}

// -----------------------------------------
// Assembly
// -----------------------------------------
void HeatSystem1D::assemble()
{
    HeatEquationModel model;
    model.k  = problem_.k;
    model.Su = problem_.Su;
    model.Sp = problem_.Sp;

    // -------------------------
    // interior
    // -------------------------
    FiniteVolumeOperator::assemble(mesh_, model, sys_);

    // -------------------------
    // boundary faces
    // -------------------------
    for (const auto face : mesh_.boundaryFaces())
    {
        int owner = mesh_.faceOwner(face);

        BoundaryContext ctx = mesh_.boundaryContext(owner, face);

        // match BC to this face
        for (const auto& bc : problem_.bcs)
        {
            if (bc.face != face)
                continue;

            switch (bc.type)
            {
                case BCType::Dirichlet:
                    applyDirichlet(bc, ctx);
                    break;

                case BCType::Neumann:
                    applyNeumann(bc, ctx);
                    break;

                case BCType::Convective:
                    applyConvective(bc, ctx);
                    break;

                default:
                    throw std::runtime_error("Unknown BC type");
            }
        }
    }
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
            return TDMA(sys_, mesh_.size(), false);

        default:
            throw std::runtime_error("Unknown solver method");
    }
}

// -----------------------------------------
// Interface methods
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

// -----------------------------------------
// Polymorphic BC interface (currently unused path)
// -----------------------------------------
// void HeatSystem1D::addBC(std::unique_ptr<BoundaryCondition> bc)
// {
//     bcs_.push_back(std::move(bc));
// }
