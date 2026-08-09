#include "discretization/operators/DiffusionOperator.hpp"

DiffusionOperator::DiffusionOperator(
    const DiffusionScheme& scheme
)
:
    scheme_(scheme)
{}

void DiffusionOperator::assemble(
    const FluxAccumulator& flux,
    EquationSystem& sys
) const
{
    for (const auto& f : flux.diffusion())
    {
        scheme_.assemble(f, sys);
    }
}