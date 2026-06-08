#pragma once
#include "mesh/primitives/Face.hpp"

struct Cell
{
    std::size_t id;

    std::vector<std::size_t> faces;   // adjacency
    Point center;

    double volume = 0.0;

    // optional: cached coefficients if you want later
};

