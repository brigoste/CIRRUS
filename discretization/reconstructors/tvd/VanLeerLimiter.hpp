#pragma once

#include "discretization/reconstructors/tvd/FluxLimiter.hpp"

class VanLeerLimiter : public FluxLimiter
{
public:

    double limit(double r) const override;
};
