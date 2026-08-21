#pragma once

#include <memory>

#include "discretization/reconstructors/ReconstructionScheme.hpp"
#include "discretization/reconstructors/ReconstructionType.hpp"

#include "discretization/reconstructors/tvd/TVDReconstruction.hpp"
#include "discretization/reconstructors/tvd/MinmodLimiter.hpp"
#include "discretization/reconstructors/tvd/FluxLimiterType.hpp"
#include "discretization/reconstructors/tvd/FluxLimiterFactory.hpp"

std::unique_ptr<ReconstructionScheme> makeReconstructionScheme(ReconstructionType type, FluxLimiterType limiterType = FluxLimiterType::Minmod);
