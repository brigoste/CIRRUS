#pragma once

#include "discretization/reconstructors/ReconstructionScheme.hpp"
#include <memory>

class FluxLimiter;

class TVDReconstruction : public ReconstructionScheme
{
public:

    explicit TVDReconstruction(
        std::unique_ptr<FluxLimiter> limiter
    );

    ReconstructionStencil stencil(
        const MeshBase& mesh,
        std::size_t owner,
        std::size_t f,
        const ScalarField& field,
        const VectorField& gradient,
        double flux
    ) const override;

    double reconstruct(
        const MeshBase& mesh,
        std::size_t owner,
        std::size_t f,
        const ScalarField& field,
        const VectorField& gradient,
        double flux
    ) const override;

private:

    std::unique_ptr<FluxLimiter> limiter_;
};