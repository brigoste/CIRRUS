#pragma once

#include <memory>

#include "discretization/reconstructors/ReconstructionScheme.hpp"
#include "discretization/reconstructors/ReconstructionType.hpp"

std::unique_ptr<ReconstructionScheme> makeReconstructionScheme(ReconstructionType type);
