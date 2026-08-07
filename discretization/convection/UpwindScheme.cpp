#include "UpwindScheme.hpp"

#include <algorithm>

void UpwindScheme::assemble(
    const FaceConvection& face,
    LinearSystem& sys
) const
{
    const auto P = face.P;
    const auto N = face.N;
    const double F = face.F;

    const double Fp = std::max(F, 0.0);
    const double Fn = std::max(-F, 0.0);

    sys.addCoeff(P, P,  Fp);
    sys.addCoeff(P, N, -Fn);

    sys.addCoeff(N, N,  Fn);
    sys.addCoeff(N, P, -Fp);
}