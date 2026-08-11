#include "discretization/convection/UpwindScheme.hpp"

double UpwindScheme::faceCoefficient(
    const FaceConvection& face
) const
{
    return face.F;
}
