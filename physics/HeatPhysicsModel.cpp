#include "HeatPhysicsModel.hpp"
#include "mesh/Face.hpp"

double HeatPhysicsModel::diffusionCoeff(const Face& face, double d) const
{
    return k_ * face.area / d;
}

double HeatPhysicsModel::convectionCoeff(double flux) const
{
    return flux;   // 0 if no convection
}
double HeatPhysicsModel::reconstructBoundaryValue(
                                                const BoundaryPatchSystem::Condition& bc,
                                                double phiCell,
                                                double dx,
                                                bool /*isLeft*/) const
{
    const double k = wallConductivity();

    switch (bc.type)
    {
        case bc::Type::Dirichlet:
            return bc.value;

        case bc::Type::Neumann:
            return phiCell + bc.flux * dx / k;

        case bc::Type::Convective:
            return (k * phiCell + bc.h * dx * bc.Tinf)
                   / (k + bc.h * dx);

        default:
            throw std::runtime_error("Unsupported BC type");
    }
}
