#include "CentralDifferenceScheme.hpp"


void CentralDifferenceScheme::assemble(
    const FaceConvection& face,
    LinearSystem& sys
) const
{
    const auto P = face.P;
    const auto N = face.N;
    const double F = face.F;

    const double halfF = 0.5 * F;

    sys.addCoeff(P, P,  halfF);
    sys.addCoeff(P, N,  halfF);

    sys.addCoeff(N, P, -halfF);
    sys.addCoeff(N, N, -halfF);
}