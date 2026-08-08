#include "discretization/diffusion/DiffusionOperator.hpp"
#include "mesh/primitives/Face.hpp"

#include <stdexcept>

void DiffusionOperator::assemble(
    const FluxAccumulator& flux,
    EquationSystem& sys
) const
{
    for (const auto& f : flux.diffusion())
    {
        const auto P = f.P;
        const auto N = f.N;
        const double D = f.D;

        if (f.type == FaceType::Interior)
        {
            sys.addCoeff(P, P,  D);
            sys.addCoeff(P, N, -D);

            sys.addCoeff(N, N,  D);
            sys.addCoeff(N, P, -D);
        }
    }
}