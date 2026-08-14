#include "discretization/reconstructors/higher_order/QuickReconstruction.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Cell.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>

ReconstructionStencil QuickReconstruction::stencil(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& /*field*/,
    const VectorField& /*gradient*/,
    double flux
) const
{
    const Face& face = mesh.face(f);

    if (face.neighbor == Face::INVALID)
    {
        throw std::runtime_error(
            "QuickReconstruction: boundary face encountered."
        );
    }

    const std::size_t upwind =
        flux >= 0.0 ? owner : face.neighbor;

    const std::size_t downwind =
        flux >= 0.0 ? face.neighbor : owner;

    const Point& xU = mesh.cellCenter(upwind);
    const Point& xD = mesh.cellCenter(downwind);

    const Vector direction = xD - xU;

    const double direction2 = direction.magnitudeSquared();

    if (direction2 <= 0.0)
    {
        throw std::runtime_error(
            "QuickReconstruction: invalid upwind/downwind spacing."
        );
    }

    const auto& upwindCell = mesh.cell(upwind);

    std::size_t upstream = Face::INVALID;
    double bestProjection = 0.0;

    // Find the cell immediately upstream of the upwind cell.
    for (const std::size_t faceIndex : upwindCell.faces)
    {
        if (faceIndex == f)
        {
            continue;
        }

        const Face& candidateFace = mesh.face(faceIndex);

        std::size_t candidate = Face::INVALID;

        if (candidateFace.owner == upwind)
        {
            candidate = candidateFace.neighbor;
        }
        else if (candidateFace.neighbor == upwind)
        {
            candidate = candidateFace.owner;
        }

        if (candidate == Face::INVALID)
        {
            continue;
        }

        const Vector dUC =
            mesh.cellCenter(candidate) - xU;

        const double projection =
            LA::dot(dUC, direction);

        if (projection < 0.0)
        {
            const double magnitude = -projection;

            if (magnitude > bestProjection)
            {
                bestProjection = magnitude;
                upstream = candidate;
            }
        }
    }

    if (upstream == Face::INVALID)
    {
        // QUICK requires a second upstream cell.
        // Near an inflow boundary, fall back to first-order upwind.
        return ReconstructionStencil{
            {
                {upwind, 1.0}
            }
        };
    }

    return ReconstructionStencil{
        {
            {upstream, -1.0 / 8.0},
            {upwind,    6.0 / 8.0},
            {downwind,  3.0 / 8.0}
        }
    };
}
