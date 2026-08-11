#include "StandardDiffusionScheme.hpp"

#include "discretization/FluxAccumulator.hpp"

void StandardDiffusionScheme::assemble(
    const FaceDiffusion& face,
    FluxAccumulator& flux
) const
{
    const auto P = face.P;
    const auto N = face.N;
    const double D = face.D;

    flux.addMatrixContribution({P, P, D});
    flux.addMatrixContribution({P, N, -D});

    flux.addMatrixContribution({N, N,  D});
    flux.addMatrixContribution({N, P, -D});
}
