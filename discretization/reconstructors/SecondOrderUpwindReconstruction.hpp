#pragma once

#include "discretization/reconstructors/ReconstructionScheme.hpp"

class SecondOrderUpwindReconstruction : public ReconstructionScheme
{
public:

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
};
