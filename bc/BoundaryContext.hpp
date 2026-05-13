#pragma once

#include "mesh/NeighborDir.hpp"

struct BoundaryContext
{
    int owner;       // owning cell
    int neighbor;    // -1 if boundary

    double area;
    double distance;

    NeighborDir normalDir;
};
