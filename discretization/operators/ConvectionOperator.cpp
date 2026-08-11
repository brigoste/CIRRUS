#include "discretization/operators/ConvectionOperator.hpp"

#include "discretization/FluxAccumulator.hpp"
#include "equation_systems/EquationSystem.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/primitives/Face.hpp"

#include <stdexcept>
#include <cmath>

ConvectionOperator::ConvectionOperator(
        const ConvectionScheme& convection,
        const ReconstructionScheme& reconstruction)
    : 
    convection_(convection), 
    reconstruction_(reconstruction) 
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

        const double F = convection_.faceCoefficient(f);

        if (std::abs(F) < 1e-14) { continue; }     // zero-flux short circuit

        const Face& face = mesh.face(f.face);

        const ReconstructionStencil stencil =
            reconstruction_.stencil(
                mesh,
                f.P,
                face,
                field,
                gradient
            );

        for (const auto& [cell, weight] : stencil.weights)
        {
            const double coefficient = F * weight;

            sys.addCoeff(f.P, cell,  coefficient);
            sys.addCoeff(f.N, cell, -coefficient);
        }
    }
}
