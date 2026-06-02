#include "discretization/FluxAccumulator.hpp"
#include "mesh/MeshBase.hpp"
#include "physics/HeatEquationModel.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "discretization/FluxBuilder.hpp"
#include <limits>

void FluxBuilder::buildFlux(
    const MeshBase& mesh,
    const HeatEquationModel& model,
    const BoundaryPatchSystem& boundary,
    FluxAccumulator& flux)
{
    const std::size_t INVALID = MeshTypes::INVALID;

    // =========================================================
    // DO NOT reallocate or reconstruct flux here
    // =========================================================

    // Optional safety check (recommended)
    if (flux.size() != mesh.ncells())
        throw std::runtime_error("FluxAccumulator size mismatch with mesh");

    // =========================================================
    // 1. FACE LOOP
    // =========================================================
    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        const double D = model.k * face.area / face.d;

        // -------------------------
        // Interior face
        // -------------------------
        if (N != INVALID)
        {
            flux.addDiffusion(P, N, D);
        }
        // -------------------------
        // Boundary face
        // -------------------------
        else
        {
            const auto* bc = boundary.get(f);

            if (!bc)
                throw std::runtime_error(
                    "Missing BC for face " + std::to_string(f));

            switch (bc->type)
            {
                case bc::Type::Dirichlet:
                    flux.addSource(P, D * bc->value, -D);
                    break;

                case bc::Type::Neumann:
                    flux.addSource(P, bc->flux * face.area, 0.0);
                    break;

                case bc::Type::Convective:
                {
                    const double hA = bc->h * face.area;
                    flux.addSource(P, hA * bc->Tinf, -hA);
                    break;
                }

                default:
                    throw std::runtime_error("Unknown BC type");
            }
        }
    }


    // =========================================================
    // 2. DEBUG CHECKS
    // =========================================================
#ifdef DEBUG
    for (std::size_t i = 0; i < flux.size(); ++i)
    {
        const auto& c = flux[i];

        if (!std::isfinite(c.Su) || !std::isfinite(c.Sp))
            throw std::runtime_error("Non-finite flux source term");

        // important diagnostic: ensures no accidental zero diagonals
        if (std::abs(c.Sp) < 1e-14)
        {
            std::cerr << "[WARN] Cell " << i
                      << " has near-zero Sp\n";
        }
    }
#endif
}
