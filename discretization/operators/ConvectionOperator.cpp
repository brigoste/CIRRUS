#include "discretization/operators/ConvectionOperator.hpp"

#include "discretization/FluxAccumulator.hpp"
#include "equation_systems/EquationSystem.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/primitives/Face.hpp"

#include <stdexcept>
#include <cmath>

ConvectionOperator::ConvectionOperator(
        const ReconstructionScheme& reconstruction)
    : 
    reconstruction_(reconstruction) 
{}

void ConvectionOperator::assemble(
    const MeshBase& mesh,
    FluxAccumulator& flux,
    const ScalarField& field,
    const VectorField& gradient
) const
{
    for (const auto& f : flux.convection())
    {
        if (f.N == Face::INVALID) { throw std::runtime_error( "ConvectionOperator: boundary face encountered." ); }

        const double F = f.F;

        if (std::abs(F) < 1e-14)
            continue;

        const Face& face = mesh.face(f.face);

        const ReconstructionStencil stencil =
            reconstruction_.stencil(
                mesh,
                f.P,
                face,
                field,
                gradient,
                F
            );

        for (const auto& [cell, weight] : stencil.weights)
        {
            const double coefficient = F * weight;

            flux.addMatrixContribution({f.P, cell, coefficient});
            flux.addMatrixContribution({f.N, cell, -coefficient});
        }
    }
}
