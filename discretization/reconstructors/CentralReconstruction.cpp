#include "discretization/reconstructors/CentralReconstruction.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Point.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>

ReconstructionStencil CentralReconstruction::stencil(
    const MeshBase& mesh,
    std::size_t owner,
    const Face& face,
    const ScalarField& /*field*/,
    const VectorField& /*gradient*/
) const
{
    if (face.neighbor == Face::INVALID) { throw std::runtime_error( "CentralReconstruction: boundary face encountered." ); }

    const Point& xP = mesh.cellCenter(owner);
    const Point& xF = face.center;
    const Vector& dPN = face.dPN;

    const double dPN2 = LA::dot(dPN, dPN);

    if (dPN2 <= 0.0) { throw std::runtime_error( "CentralReconstruction: invalid owner-neighbor spacing." ); }

    const Vector dPF = xF - xP;

    const double alpha = LA::dot(dPF, dPN) / dPN2;

    return ReconstructionStencil{
        {
            {owner, 1.0 - alpha},
            {face.neighbor, alpha}
        }
    };
}

double CentralReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    const Face& face,
    const ScalarField& field,
    const VectorField& /*gradient*/
) const
{
    if (face.neighbor == Face::INVALID) { throw std::runtime_error( "CentralReconstruction: boundary face encountered." ); }

    const Point& xP = mesh.cellCenter(owner);
    const Point& xF = face.center;

    const Vector& dPN = face.dPN;

    const double dPN2 = LA::dot(dPN, dPN);

    if (dPN2 <= 0.0) { throw std::runtime_error( "CentralReconstruction: invalid owner-neighbor spacing." ); }

    const Vector dPF = xF - xP;

    const double alpha = LA::dot(dPF, dPN) / dPN2;

    return (1.0 - alpha) * field[owner] + alpha * field[face.neighbor];
}
