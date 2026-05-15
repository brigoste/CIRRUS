#pragma once
#include <stdexcept>

struct FaceConvection
{
    std::size_t P, N;
    double F;   // mass flux = rho u·n A
};
