#include "discretization/reconstructors/basic/UpwindReconstruction.hpp"

#include "mesh/primitives/Face.hpp"

#include <cmath>
#include <stdexcept>
#include <iostream>

ReconstructionStencil UpwindReconstruction::stencil(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& /*field*/,
    const VectorField& /*gradient*/,
    double flux
) const
{
    const Face& face = mesh.face(f);
    if (face.neighbor == Face::INVALID) { throw std::runtime_error( "UpwindReconstruction: boundary face encountered." ); }

    if (flux > 0.0) { return ReconstructionStencil{ { {owner, 1.0} } }; }

    if (flux < 0.0) { return ReconstructionStencil{ { {face.neighbor, 1.0} } }; }

    return ReconstructionStencil{};
}

double UpwindReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& field,
    const VectorField& /*gradient*/,
    double flux
) const
{
    const Face& face = mesh.face(f);
    if (face.neighbor == Face::INVALID) { throw std::runtime_error( "UpwindReconstruction: boundary face encountered." ); }

    if (flux >= 0.0) { return field[owner]; }

    return field[face.neighbor];
}
