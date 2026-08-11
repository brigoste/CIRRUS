#include "discretization/convection/CentralDifferenceScheme.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"

#include "utils/LinearAlgebraUtils.hpp"

#include <cmath>
#include <stdexcept>

ConvectionCoefficients CentralDifferenceScheme::coefficients(
    const MeshBase& mesh,
    const FaceConvection& face,
    const ScalarField& /*field*/,
    const VectorField& /*gradient*/
) const
{
    const Face& f = mesh.face(face.face);
    const Point& xP = mesh.cellCenter(face.P);

    const double dPN =
        std::abs(LA::dot(f.dPN, f.normal));

    if (dPN <= 0.0)
    {
        throw std::runtime_error(
            "CentralDifferenceScheme: invalid face spacing."
        );
    }

    const double dPF =
        std::abs(LA::dot(f.center - xP, f.normal));

    const double alpha = dPF / dPN;

    return {
        face.F * (1.0 - alpha),
        face.F * alpha
    };
}