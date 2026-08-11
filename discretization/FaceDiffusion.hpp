#pragma once

#include "discretization/FaceContribution.hpp"

struct FaceDiffusion : FaceContribution
{
    double D = 0.0;
};
