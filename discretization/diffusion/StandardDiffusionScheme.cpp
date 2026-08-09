#include "StandardDiffusionScheme.hpp"

void StandardDiffusionScheme::assemble(
    const FaceDiffusion& face,
    EquationSystem& sys
) const
{
    const auto P = face.P;
    const auto N = face.N;
    const double D = face.D;

    sys.addCoeff(P, P,  D);
    sys.addCoeff(P, N, -D);

    sys.addCoeff(N, N,  D);
    sys.addCoeff(N, P, -D);
}