#include <memory>
#include "discretization/interpolators/InterpolationType.hpp"
#include "discretization/interpolators/InterpolationScheme.hpp"
#include "discretization/interpolators/CentralLinearInterpolation.hpp"
#include "discretization/interpolators/UpwindInterpolation.hpp"

std::unique_ptr<InterpolationScheme> makeInterpolationScheme(InterpolationType type);