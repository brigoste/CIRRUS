#pragma once

#include "discretization/reconstructors/tvd/FluxLimiter.hpp"

class MinmodLimiter : public FluxLimiter
{
public:

    double limit(double r) const override;
};