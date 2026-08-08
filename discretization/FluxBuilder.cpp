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
    const VerificationCase* verificationCase)
{
    if (flux.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "FluxAccumulator size mismatch"
        );
    }

    // =====================================================
    // 1. DIFFUSION
    // =====================================================
    diffusion_.apply(
        mesh,
        model,
        boundary,
        flux,
        verificationCase
    );

    // =====================================================
    // 2. INTERIOR CONVECTION
    // =====================================================
    convectionFlux_.apply(
        mesh,
        model,
        flux
    );

    // =====================================================
    // 3. CELL SOURCES
    // =====================================================
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const double source = model.cellSource(mesh, c);
        // Physical model source
        flux.addSource(
            c,
            source * mesh.cellVolume(c),
            0.0
        );

        // Manufactured verification forcing
        if (verificationCase)
        {
            const auto& xc = mesh.cellCenter(c);

            const double manufacturedSource =
                verificationCase->source(
                    xc.x[0],
                    xc.x[1]
                );

            flux.addSource(
                c,
                manufacturedSource * mesh.cellVolume(c),
                0.0
            );
        }
    }
}