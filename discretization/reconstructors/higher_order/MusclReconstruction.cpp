#include "discretization/reconstructors/higher_order/MusclReconstruction.hpp"
#include "discretization/reconstructors/ReconstructionUtils.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>

ReconstructionStencil MusclReconstruction::stencil(
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

    // -------------------------------------------------
    // Boundary fallback
    // -------------------------------------------------

    if (cells.upstream == Face::INVALID)
    {
        // Fall back to first-order upwind reconstruction.
        return ReconstructionStencil{
            {
                {cells.upwind, 1.0}
            }
        };
    }

    // -------------------------------------------------
    // Unlimited linear MUSCL stencil
    // -------------------------------------------------

    // return ReconstructionStencil{
    //     {
    //         {cells.upstream, -0.25},
    //         {cells.upwind,     0.75},
    //         {cells.downwind,   0.50}
    //     }
    // };
    return ReconstructionStencil{
    {
        {cells.upstream, -0.125},
        {cells.upwind,    0.75},
        {cells.downwind,  0.375}
    }
};
}

double MusclReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& field,
    const VectorField& gradient,
    double flux
) const
{
    const ReconstructionStencil s =
        stencil(
            mesh,
            owner,
            f,
            field,
            gradient,
            flux
        );

    double value = 0.0;

    for (const auto& [cell, weight] : s.weights)
    {
        value += weight * field[cell];
    }

    return value;
}
