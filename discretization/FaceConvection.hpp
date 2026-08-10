#pragma once

#include <cstddef>

struct FaceConvection
{
    std::size_t P;
    std::size_t N;
    std::size_t face;
    double F;
};