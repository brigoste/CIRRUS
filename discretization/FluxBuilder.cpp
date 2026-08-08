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
    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        if (N != Face::INVALID)
        {
            const double F =
                model.convectionFaceFlux(face);

            flux.addConvection(P, N, F);
        }
    }

    // =====================================================
    // 3. CELL SOURCES
    // =====================================================
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        // Physical model source
        model.addCellSources(mesh, c, flux);

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

#ifdef DEBUG
    for (std::size_t i = 0; i < flux.size(); ++i)
    {
        const auto& c = flux[i];

        if (!std::isfinite(c.Su) ||
            !std::isfinite(c.Sp))
        {
            throw std::runtime_error(
                "Non-finite flux source term"
            );
        }

        if (std::abs(c.Sp) > 1e12)
        {
            std::cerr
                << "[WARN] stiff source at cell "
                << i << "\n";
        }
    }
#endif
}