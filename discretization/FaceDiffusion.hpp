#pragma once
#include <stdexcept>

enum class FaceType
{
    Interior,
    Boundary
};

struct FaceDiffusion
{
    std::size_t P;
    std::size_t N;     // only valid if Interior
    double D;
    FaceType type;
};
