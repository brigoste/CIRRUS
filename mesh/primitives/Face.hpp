#pragma once
#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Vector.hpp"
#include <cstddef>

struct Face
{
    std::size_t owner;
    std::size_t neighbor;

    Point center;
    Vector normal;

    double area = 1.0;

    // // CURRENT (remove dependency eventually)
    // double d = 0.0;

    // NEW (important for 2D correctness)
    Vector dPN;   // x_N - x_P (valid only for interior faces)

    static constexpr std::size_t INVALID = static_cast<std::size_t>(-1);
};
