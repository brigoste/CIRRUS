#pragma once

#include "discretization/reconstructors/tvd/FluxLimiter.hpp"
#include "discretization/reconstructors/tvd/FluxLimiterType.hpp"

#include <memory>

std::unique_ptr<FluxLimiter> makeLimiter(FluxLimiterType type);
