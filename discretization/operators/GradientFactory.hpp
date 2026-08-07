#pragma once

#include <memory>
#include <stdexcept>

#include "discretization/gradient/GradientScheme.hpp"
#include "discretization/gradient/GradientType.hpp"

#include "discretization/gradient/GreenGaussGradient.hpp"
#include "discretization/gradient/LeastSquaresGradient.hpp"


class GradientFactory
{
public:

static std::unique_ptr<GradientScheme> create(
    GradientType type
)
{
    switch(type)
    {
        case GradientType::GreenGauss:
            return std::make_unique<GreenGaussGradient>();

        case GradientType::LeastSquares:
            return std::make_unique<LeastSquaresGradient>();
    }

    throw std::runtime_error(
        "Invalid gradient type"
    );
}

};