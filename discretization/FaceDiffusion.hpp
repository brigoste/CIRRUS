#pragma once

#include <cstddef>
#include "discretization/FaceType.hpp"

struct FaceDiffusion
{
    std::size_t P;
    std::size_t N;
    double D;
    FaceType type;
};