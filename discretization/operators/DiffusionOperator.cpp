#include "discretization/operators/DiffusionOperator.hpp"
#include "discretization/FluxAccumulator.hpp"

DiffusionOperator::DiffusionOperator(
    const DiffusionScheme& scheme
)
:
    scheme_(scheme)
{}

void DiffusionOperator::assemble(
    const MeshBase& /*mesh*/,
    FluxAccumulator& flux
) const
{
    for (const auto& f : flux.diffusion())
    {
        scheme_.assemble(f, flux);
    }
}
