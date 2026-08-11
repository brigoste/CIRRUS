#pragma once

#include <memory>

#include "discretization/convection/ConvectionScheme.hpp"
#include "discretization/convection/ConvectionType.hpp"

std::unique_ptr<ConvectionScheme> makeConvectionScheme(ConvectionType type);