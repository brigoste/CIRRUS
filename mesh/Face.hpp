#pragma once
#include "mesh/Point.hpp"
#include <cstddef>

struct Face
{
    std::size_t owner;
    std::size_t neighbor;

    Point center;
    Point normal;

    double area = 1.0;

    // geometric distance between owner and neighbor cell centers (INTERIOR ONLY)
    double dPN = 0.0;

    static constexpr std::size_t INVALID = static_cast<std::size_t>(-1);
};
