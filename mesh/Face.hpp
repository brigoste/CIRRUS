#pragma once
#include "mesh/MeshTypes.hpp"
#include "mesh/Point.hpp"

struct Face {
    std::size_t owner;
    std::size_t neighbor;   // INVALID = boundary

    Point center;
    Point normal;

    double area;
    double d;

    static constexpr std::size_t INVALID = static_cast<std::size_t>(-1);
};
