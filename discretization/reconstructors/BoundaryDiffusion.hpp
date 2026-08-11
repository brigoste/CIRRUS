#pragma once

#include "discretization/FaceContribution.hpp"

struct BoundaryDiffusion : FaceContribution
{
    double D = 0.0;
    double value = 0.0;
};
