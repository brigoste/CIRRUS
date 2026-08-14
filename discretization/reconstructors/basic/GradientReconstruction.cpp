#include "discretization/reconstructors/basic/GradientReconstruction.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Point.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

double GradientReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t f,
    const ScalarField& field,
    const VectorField& gradient,
    double /*flux*/
) const
{
    const Face& face = mesh.face(f);
    const Point& xP = mesh.cellCenter(owner);
    const Point& xF = face.center;

    const Vector displacement = xF - xP;

    return field[owner] + LA::dot(displacement, gradient[owner]);
}

// ReconstructionStencil GradientReconstruction::stencil(
//         const MeshBase& mesh,
//         std::size_t owner,
//         std::size_t f,
//         const ScalarField& field,
//         const VectorField& gradient
//     ) const
// {
//     return;
// }
