#include "discretization/convection/UpwindScheme.hpp"

ConvectionCoefficients UpwindScheme::coefficients(
const MeshBase& /*mesh*/,
const FaceConvection& face,
const ScalarField& /*field*/,
const VectorField& /*gradient*/
) const
{
    if (face.F >= 0.0)
    {
        // Flow from owner -> neighbor.
        return { face.F, 0.0 };
    }

    // Flow from neighbor -> owner.
    return { 0.0, face.F };
}