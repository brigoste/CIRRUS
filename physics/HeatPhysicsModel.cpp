#include "HeatPhysicsModel.hpp"
#include "mesh/primitives/Face.hpp"
#include "utils/LinearAlgebraUtils.hpp"

double HeatPhysicsModel::diffusionCoeff(const Face& face) const
{
    double d_eff = std::abs(LA::dot(face.dPN, face.normal));
    if (d_eff <= 0.0)
    {
        throw std::runtime_error(
            "Invalid face spacing: d_eff = " +
            std::to_string(d_eff));
    }
    return k_ * face.area / d_eff;
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
