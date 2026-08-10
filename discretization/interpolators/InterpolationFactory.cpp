#include "discretization/interpolators/InterpolationFactory.hpp"

std::unique_ptr<InterpolationScheme>
makeInterpolationScheme(InterpolationType type)
{
    switch (type)
    {
        case InterpolationType::CentralLinear:
            return std::make_unique<CentralLinearInterpolation>();

        case InterpolationType::Upwind:
            return std::make_unique<UpwindInterpolation>();
    }

    throw std::runtime_error(
        "Unsupported interpolation scheme."
    );
}