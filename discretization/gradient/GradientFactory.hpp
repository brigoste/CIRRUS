#pragma once

#include <memory>

#include "discretization/gradient/GradientScheme.hpp"
#include "discretization/gradient/GradientType.hpp"

std::unique_ptr<GradientScheme> makeGradientScheme(GradientType type);