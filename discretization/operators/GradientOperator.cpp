#include "GradientOperator.hpp"

GradientOperator::GradientOperator(
    std::unique_ptr<GradientScheme> scheme
)
:
    scheme_(std::move(scheme))
{
    if (!scheme_)
    {
        throw std::runtime_error(
            "GradientOperator requires a valid GradientScheme"
        );
    }
}


void GradientOperator::compute(
    const MeshBase& mesh,
    const ScalarField& field,
    VectorField& gradient
) const
{
    scheme_->compute(
        mesh,
        field,
        gradient
    );
}