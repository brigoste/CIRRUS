#pragma once

#include "discretization/reconstructors/tvd/FluxLimiter.hpp"

class MCLimiter : public FluxLimiter
{
public:

    double limit(double r) const override;
};
