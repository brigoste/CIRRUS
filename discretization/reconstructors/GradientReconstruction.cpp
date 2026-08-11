#include "discretization/reconstructors/GradientReconstruction.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Point.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

double GradientReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    const Face& face,
    const ScalarField& field,
    const VectorField& gradient
) const
{
    const Point& xP = mesh.cellCenter(owner);
    const Point& xF = face.center;

    const Vector displacement = xF - xP;

    return field[owner] + LA::dot(displacement, gradient[owner]);
}

// ReconstructionStencil GradientReconstruction::stencil(
//         const MeshBase& mesh,
//         std::size_t owner,
//         const Face& face,
//         const ScalarField& field,
//         const VectorField& gradient
//     ) const
// {
//     return;
// }
