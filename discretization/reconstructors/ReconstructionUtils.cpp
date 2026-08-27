#include "ReconstructionUtils.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Cell.hpp"
#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>

UpwindStencilCells findUpwindStencilCells(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t faceIndex,
    double flux
)
{
    const Face& face = mesh.face(faceIndex);

    if (face.neighbor == Face::INVALID) { throw std::runtime_error( "ReconstructionUtils: boundary face encountered." ); }

    const std::size_t upwind = flux >= 0.0 ? owner : face.neighbor;
    const std::size_t downwind = flux >= 0.0 ? face.neighbor : owner;

    const Point& xU = mesh.cellCenter(upwind);
    const Point& xD = mesh.cellCenter(downwind);

    const Vector direction = xD - xU;

    const double direction2 = direction.magnitudeSquared();

    if (direction2 <= 0.0) { throw std::runtime_error( "ReconstructionUtils: invalid upwind/downwind spacing." ); }

    const auto& upwindCell = mesh.cell(upwind);

    std::size_t upstream = Face::INVALID;
    double bestProjection = 0.0;

    for (const std::size_t candidateFaceIndex : upwindCell.faces)
    {
        if (candidateFaceIndex == faceIndex) { continue; }

        const Face& candidateFace = mesh.face(candidateFaceIndex);

        std::size_t candidate = Face::INVALID;

        if (candidateFace.owner == upwind) { candidate = candidateFace.neighbor; }
        else if (candidateFace.neighbor == upwind) { candidate = candidateFace.owner; }

        // Boundary face or otherwise invalid connectivity.
        if (candidate == Face::INVALID) { continue; }

        const Vector dUC = mesh.cellCenter(candidate) - xU;
        const double projection = LA::dot(dUC, direction);

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

    return UpwindStencilCells{
        upwind,
        downwind,
        upstream
    };
}
