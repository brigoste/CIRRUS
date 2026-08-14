#include "SecondOrderUpwindReconstruction.hpp"

#include "discretization/reconstructors/ReconstructionUtils.hpp"

#include "mesh/MeshBase.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>

ReconstructionStencil SecondOrderUpwindReconstruction::stencil(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& /*field*/,
    const VectorField& /*gradient*/,
    double flux
) const
{
    const UpwindStencilCells cells =
        findUpwindStencilCells(
            mesh,
            owner,
            f,
            flux
        );

    if (cells.upstream == Face::INVALID)
    {
        // Fall back to first-order upwind reconstruction.
        return ReconstructionStencil{
            {
                {cells.upwind, 1.0}
            }
        };
    }

    return ReconstructionStencil{
        {
            {cells.upwind, 1.5},
            {cells.upstream, -0.5}
        }
    };
}

double SecondOrderUpwindReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& field,
    const VectorField& gradient,
    double flux
) const
{
    const Face& face = mesh.face(f);

    if (face.neighbor == Face::INVALID)
    {
        throw std::runtime_error( "SecondOrderUpwindReconstruction: boundary face encountered." );
    }

    const std::size_t upwind = flux >= 0.0 ? owner : face.neighbor;

    const Point& xU = mesh.cellCenter(upwind);
    const Point& xF = face.center;

    const Vector dUF = xF - xU;

    return field[upwind] + LA::dot(gradient[upwind], dUF);
}
