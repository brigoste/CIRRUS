#include "discretization/builders/ConvectionFluxBuilder.hpp"

#include <cmath>

void ConvectionFluxBuilder::apply(
    const MeshBase& mesh,
    const PhysicsModel& model,
    const BoundaryPatchSystem& boundary,
    FluxAccumulator& flux
) const
{
    // =====================================================
    // 1. INTERIOR FACES
    // =====================================================

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        if (N != Face::INVALID)
        {
            const double F = model.convectionFaceFlux(face);

            if (std::abs(F) > 1e-14) { flux.addConvection({P, N, f, F}); }
        }
    }

    // =====================================================
    // 2. BOUNDARY FACES
    // =====================================================

    for (std::size_t g = 0; g < mesh.nBoundaryGroups(); ++g)
    {
        const auto* bc = boundary.getGroup(g);

        if (!bc) { continue; }

        for (std::size_t f : mesh.boundaryFaces(g))
        {
            const Face& face = mesh.face(f);
            const std::size_t P = face.owner;

            const double F = model.convectionFaceFlux(face);

            if (std::abs(F) < 1e-14) { continue; }

            // -------------------------------------------------
            // Inflow
            // -------------------------------------------------

            if (F < 0.0) { flux.addSource({ P, -F * bc->value, 0.0 }); }

            // -------------------------------------------------
            // Outflow
            // -------------------------------------------------

            else { flux.addMatrixContribution({ P, P, F }); }
        }
    }
}