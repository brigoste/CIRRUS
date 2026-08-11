#pragma once

#include <cstddef>

class MeshBase;
class ScalarField;
class VectorField;
struct Face;

class ReconstructionScheme
{
public:

    virtual ~ReconstructionScheme() = default;

    virtual double reconstruct(
        const MeshBase& mesh,
        std::size_t owner,
        const Face& face,
        const ScalarField& field,
        const VectorField& gradient
    ) const = 0;
};
