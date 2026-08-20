#pragma once

#include "discretization/reconstructors/tvd/FluxLimiter.hpp"

class VanAlbadaLimiter : public FluxLimiter
{
public:

    double limit(double r) const override;
};
