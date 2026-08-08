#include "discretization/FluxAccumulator.hpp"
#include "physics/PhysicsModel.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "discretization/FluxBuilder.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

void FluxBuilder::buildFlux(
    const MeshBase& mesh,
    const PhysicsModel& model,
    const BoundaryPatchSystem& boundary,
    FluxAccumulator& flux,
    const VerificationCase* verificationCase
) const
{
    if (flux.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "FluxAccumulator size mismatch"
        );
    }

    diffusionFlux_.apply(
        mesh,
        model,
        boundary,
        flux,
        verificationCase
    );

    convectionFlux_.apply(
        mesh,
        model,
        flux
    );

    sourceFlux_.apply(
        mesh,
        model,
        flux,
        verificationCase
    );
}