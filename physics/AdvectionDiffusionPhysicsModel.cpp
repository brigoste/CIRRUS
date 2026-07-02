#include "physics/AdvectionDiffusionPhysicsModel.hpp"
#include "utils/MathConstants.hpp"

#include <cmath>
#include <stdexcept>


double AdvectionDiffusionPhysicsModel::diffusionFaceCoefficient(
    const Face& face) const
{
    const double d_eff = std::abs(LA::dot(face.dPN, face.normal));

    if (d_eff <= 0.0)
        throw std::runtime_error("Invalid face spacing");

    return gamma_ * face.area / d_eff;
}

double AdvectionDiffusionPhysicsModel::convectionFaceFlux(
    const Face& face) const
{
    // Constant velocity field
    // Assumes face.normal is unit normal from owner -> neighbor
    const double un =
          ux_ * face.normal.x[0]
        + uy_ * face.normal.x[1]
        + uz_ * face.normal.x[2];

    return rho_ * un * face.area;
}

void AdvectionDiffusionPhysicsModel::addCellSources(
    const MeshBase& mesh,
    std::size_t c,
    FluxAccumulator& flux) const
{
    (void)mesh;
    (void)c;
    (void)flux;

    // No intrinsic volumetric source.
    // Verification sources are added separately by the verification framework.
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

        case bc::Type::Convective:
            return (gamma * phiCell + bc.h * dx * bc.Tinf) / (gamma + bc.h * dx);

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
