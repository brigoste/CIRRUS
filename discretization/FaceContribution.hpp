#pragma once

#include "mesh/primitives/Face.hpp"
#include <cstddef>

struct FaceContribution
{
    std::size_t P = 0;
    std::size_t N = Face::INVALID;
    std::size_t face = 0;
};
