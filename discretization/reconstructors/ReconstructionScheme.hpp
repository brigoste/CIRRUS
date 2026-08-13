#pragma once

#include <cstddef>
#include <stdexcept>

#include "discretization/reconstructors/ReconstructionStencil.hpp"
#include "fields/ScalarField.hpp"

class MeshBase;
class VectorField;
struct Face;

class ReconstructionScheme
{
public:

    virtual ~ReconstructionScheme() = default;

    virtual ReconstructionStencil stencil(      // returns coefficients such that φ_f = Σ w_i φ_i
        const MeshBase& /*mesh*/,
        std::size_t /*owner*/,
        std::size_t /*faceIndex*/,
        const ScalarField& /*field*/,
        const VectorField& /*gradient*/,
        double /*flux*/
    ) const
    {
        throw std::runtime_error( "Reconstruction scheme does not provide a cell-value stencil." );
    }

    virtual double reconstruct(                 // directly evaluates φ_f     ---> May not be necessary
        const MeshBase& mesh,
        std::size_t owner,
        std::size_t f,
        const ScalarField& field,
        const VectorField& gradient,
        double flux
    ) const
    {
        const auto stencil = this->stencil(mesh, owner, f, field, gradient, flux);

        double value = 0.0;

        for (const auto& [cell, weight] : stencil.weights) { value += weight * field[cell]; }

        return value;
    }      
};
