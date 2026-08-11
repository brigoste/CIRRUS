#pragma once

#include <cstddef>

#include "discretization/reconstructors/ReconstructionScheme.hpp"

class GradientReconstruction : public ReconstructionScheme
{
public:

    double reconstruct(
        const MeshBase& mesh,
        std::size_t owner,
        const Face& face,
        const ScalarField& field,
        const VectorField& gradient
    ) const override;
};
