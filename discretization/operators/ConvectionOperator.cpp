#include "discretization/operators/ConvectionOperator.hpp"

#include "discretization/FluxAccumulator.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Point.hpp"
#include "equation_systems/EquationSystem.hpp"
#include "utils/LinearAlgebraUtils.hpp"

#include <cmath>
#include <stdexcept>

ConvectionOperator::ConvectionOperator(
    const InterpolationScheme& interpolation
)
    : interpolation_(interpolation)
{
}

void ConvectionOperator::assemble(
    const MeshBase& mesh,
    const FluxAccumulator& flux,
    EquationSystem& sys
) const
{
    for (const auto& f : flux.convection())
    {
        const auto P = f.P;
        const auto N = f.N;
        const double F = f.F;

        if (N == Face::INVALID)
        {
            throw std::runtime_error(
                "ConvectionOperator: boundary face encountered."
            );
        }

        const Face& face = mesh.face(f.face);
        const Point& xP = mesh.cellCenter(P);

        const double dPN = std::abs(LA::dot(face.dPN, face.normal));

        if (dPN <= 0.0)
        {
            throw std::runtime_error( "ConvectionOperator: invalid face spacing." );
        }

        const double dPF = std::abs(LA::dot(face.center - xP, face.normal));

        const double alpha = dPF / dPN;
        
        const auto weights = interpolation_.interpolate(alpha, F);

        // assemble using weights...
        sys.addCoeff(P, P,  F * weights.owner);
        sys.addCoeff(P, N,  F * weights.neighbor);

        sys.addCoeff(N, P, -F * weights.owner);
        sys.addCoeff(N, N, -F * weights.neighbor);
    }
}