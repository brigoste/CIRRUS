#include "discretization/reconstructors/GradientReconstruction.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Point.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

double GradientReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t cell,
    const Face& face,
    const ScalarField& field,
    const VectorField& gradient
) const
{
    const Point& xP = mesh.cellCenter(cell);
    const Point& xF = face.center;

    const Point displacement = xF - xP;

    return field[cell] + LA::dot(displacement, gradient[cell]);
}