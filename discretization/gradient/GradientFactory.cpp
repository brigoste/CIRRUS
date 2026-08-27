#include "discretization/gradient/GradientFactory.hpp"

#include "discretization/gradient/GreenGaussGradient.hpp"
#include "discretization/gradient/LeastSquaresGradient.hpp"

#include <stdexcept>

std::unique_ptr<GradientScheme> makeGradientScheme(GradientType type)
{
    switch (type)
    {
        case GradientType::GreenGauss:
            return std::make_unique<GreenGaussGradient>();

        case GradientType::LeastSquares:
            return std::make_unique<LeastSquaresGradient>();
    }

    throw std::runtime_error( "Unsupported gradient scheme." );
}
