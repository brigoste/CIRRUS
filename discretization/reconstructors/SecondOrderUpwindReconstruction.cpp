#include "SecondOrderUpwindReconstruction.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Point.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>

double SecondOrderUpwindReconstruction::reconstruct(
    const MeshBase& mesh,
    std::size_t owner,
    const Face& face,
    const ScalarField& field,
    const VectorField& gradient,
    double flux
) const
{
    if (face.neighbor == Face::INVALID)
    {
        throw std::runtime_error(
            "SecondOrderUpwindReconstruction: boundary face encountered."
        );
    }

    std::size_t upwind;

    if (flux >= 0.0)
    {
        upwind = owner;
    }
    else
    {
        upwind = face.neighbor;
    }

    const Point& xU = mesh.cellCenter(upwind);
    const Point& xF = face.center;

    const Vector dUF = xF - xU;

    return field[upwind]
         + LA::dot(gradient[upwind], dUF);
}
