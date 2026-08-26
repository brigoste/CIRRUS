#include "physics/AdvectionDiffusionPhysicsModel.hpp"
#include "utils/MathConstants.hpp"

#include <cmath>
#include <stdexcept>


double AdvectionDiffusionPhysicsModel::diffusionFaceCoefficient(
    const Face& face) const
{
    const double d_eff = std::abs(LA::dot(face.dPN, face.normal));

    if (d_eff <= 0.0) { throw std::runtime_error("Invalid face spacing"); }

    return gamma_ * face.area / d_eff;
}

double AdvectionDiffusionPhysicsModel::convectionFaceFlux(
    const Face& face) const
{
    // Constant velocity field
    // Assumes face.normal is unit normal from owner -> neighbor
    const double un =
          ux_ * face.normal.x
        + uy_ * face.normal.y
        + uz_ * face.normal.z;

    return rho_ * un * face.area;
}

double AdvectionDiffusionPhysicsModel::reconstructBoundaryValue(
    const BoundaryPatchSystem::Condition& bc,
    double phiCell,
    double dx,
    bool /*isLeft*/) const
{
    // For now, use the same diffusive-style closure as heat.
    // This is acceptable for the first scalar transport implementation.
    const double gamma = diffusionCoefficient();

    switch (bc.type)
    {
        case bc::Type::Dirichlet:
            return bc.value;

        case bc::Type::Neumann:
            return phiCell + bc.flux * dx / gamma;

        case bc::Type::Robin:
            return (gamma * phiCell + bc.transferCoefficient * dx * bc.referenceValue) / (gamma + bc.transferCoefficient * dx);

        default:
            throw std::runtime_error("Unsupported BC type");
    }
}

double AdvectionDiffusionPhysicsModel::cellSource(
    const MeshBase& mesh,
    std::size_t cell) const
{
    (void) mesh;
    (void) cell;
    return 0.0; // or proper source model
}
