#pragma once

#include "discretization/reconstructors/tvd/FluxLimiter.hpp"

class SuperbeeLimiter : public FluxLimiter
{
public:

    double limit(double r) const override;
};
