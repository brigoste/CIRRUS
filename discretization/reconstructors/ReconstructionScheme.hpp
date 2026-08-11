#pragma once

#include <cstddef>
#include <stdexcept>

#include "discretization/reconstructors/ReconstructionStencil.hpp"

class MeshBase;
class ScalarField;
class VectorField;
struct Face;

class ReconstructionScheme
{
public:

    virtual ~ReconstructionScheme() = default;

    virtual double reconstruct(                 //gives you a linear representation of that reconstruction:
        const MeshBase& mesh,
        std::size_t owner,
        const Face& face,
        const ScalarField& field,
        const VectorField& gradient
    ) const = 0;
    
    virtual ReconstructionStencil stencil(      //gives you the actual face value:
        const MeshBase& /*mesh*/,
        std::size_t /*owner*/,
        const Face& /*face*/,
        const ScalarField& /*field*/,
        const VectorField& /*gradient*/
    ) const 
    {
        throw std::runtime_error( "Reconstruction scheme does not provide a cell-value stencil." );
    }
};
