#include "TVDReconstruction.hpp"

#include "discretization/reconstructors/ReconstructionUtils.hpp"
#include "discretization/reconstructors/tvd/FluxLimiter.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include <cmath>
#include <utility>
#include <iostream>
#include <limits>
#include <algorithm>

TVDReconstruction::TVDReconstruction(
    std::unique_ptr<FluxLimiter> limiter
)
    :
    limiter_(std::move(limiter))
{
}

ReconstructionStencil TVDReconstruction::stencil(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& field,
    const VectorField& /*gradient*/,
    double flux
) const
{
    // -------------------------------------------------
    // Determine upwind stencil cells
    // -------------------------------------------------

    const UpwindStencilCells cells = findUpwindStencilCells( mesh, owner, f, flux );

    // -------------------------------------------------
    // Boundary fallback
    // -------------------------------------------------
    if (cells.upstream == Face::INVALID)
    {
        return ReconstructionStencil{
            {
                {cells.upwind, 1.0}
            }
        };
    }


    // -------------------------------------------------
    // Cell values
    // -------------------------------------------------

    const double phiUU = field[cells.upstream];
    const double phiU = field[cells.upwind];
    const double phiD = field[cells.downwind];

    // -------------------------------------------------
    // Downwind difference
    // -------------------------------------------------

    const double deltaUD = phiD - phiU;

    // -------------------------------------------------
    // Constant / locally flat field
    // -------------------------------------------------

    // -------------------------------------------------
    // Locally flat field
    //
    // No directional variation exists between the
    // upwind and downwind cells, so the limiter ratio
    // is undefined. Use first-order upwind.
    // -------------------------------------------------

    if (std::abs(deltaUD) < 1e-14)
    {
        return ReconstructionStencil{
            {
                {cells.upwind, 1.0}
            }
        };
    }

    if (std::abs(deltaUD) < 1e-14)
    {
        return ReconstructionStencil{
            {
                {cells.upwind, 1.0}
            }
        };
    }

    // -------------------------------------------------
    // Smoothness ratio
    // -------------------------------------------------

    const double deltaUU = phiU - phiUU;
    const double r = deltaUU / deltaUD;

    // -------------------------------------------------
    // Apply limiter
    // -------------------------------------------------
    
    const double psi = limiter_->limit(r);

    // -------------------------------------------------
    // Limited face value
    //
    // phi_f =
    //     phi_U
    //     + 0.5 * psi * (phi_D - phi_U)
    //
    // Therefore:
    //
    // phi_f =
    //     (1 - 0.5 psi) phi_U
    //     + (0.5 psi) phi_D
    // -------------------------------------------------

    const double upwindWeight = 1.0 - 0.5 * psi;
    const double downwindWeight = 0.5 * psi;

    return ReconstructionStencil{
        {
            {cells.upwind,   upwindWeight},
            {cells.downwind, downwindWeight}
        }
    };
}

double TVDReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& field,
    const VectorField& gradient,
    double flux
) const
{
    const ReconstructionStencil s = stencil( mesh, owner, f, field, gradient, flux );

    double value = 0.0;

    for (const auto& [cell, weight] : s.weights)
    {
        value += weight * field[cell];
    }

    return value;
}
