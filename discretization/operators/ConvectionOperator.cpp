#include "discretization/operators/ConvectionOperator.hpp"

#include "discretization/FluxAccumulator.hpp"
#include "equation_systems/EquationSystem.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/primitives/Face.hpp"

#include <stdexcept>

ConvectionOperator::ConvectionOperator( const ConvectionScheme& scheme )
    : scheme_(scheme)
{}

void ConvectionOperator::assemble(
const MeshBase& mesh,
const FluxAccumulator& flux,
const ScalarField& field,
const VectorField& gradient,
EquationSystem& sys
) const
{
    for (const auto& f : flux.convection())
    {
        if (f.N == Face::INVALID) { throw std::runtime_error( "ConvectionOperator: boundary face encountered." ); }

        const auto coefficients = scheme_.coefficients(
            mesh,
            f,
            field,
            gradient
        );

        sys.addCoeff(f.P, f.P, coefficients.owner);
        sys.addCoeff(f.P, f.N, coefficients.neighbor);

        sys.addCoeff(f.N, f.P, -coefficients.owner);
        sys.addCoeff(f.N, f.N, -coefficients.neighbor);
    }
}