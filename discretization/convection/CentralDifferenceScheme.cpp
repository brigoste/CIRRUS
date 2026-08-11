#include "discretization/convection/CentralDifferenceScheme.hpp"

double CentralDifferenceScheme::faceCoefficient(
    const FaceConvection& face
) const
{
    return face.F;
}
