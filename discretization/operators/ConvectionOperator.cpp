#include "discretization/operators/ConvectionOperator.hpp"

ConvectionOperator::ConvectionOperator(
    const ConvectionScheme& scheme
)
:
    scheme_(scheme)
{}

void ConvectionOperator::assemble(
    const FluxAccumulator& flux,
    EquationSystem& sys
) const
{
    for (const auto& f : flux.convection())
    {
        scheme_.assemble(f, sys);
    }
}