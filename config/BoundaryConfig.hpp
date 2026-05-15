#pragma once
#include "mesh/MeshBase.hpp"
#include "bc/BCType.hpp"
#include "bc/BoundaryFace.hpp"

struct BoundaryConfig
{
    BoundaryFace face;
    BCType type;

    double value = 0.0;
    double flux  = 0.0;
    double h     = 0.0;
    double Tinf  = 0.0;
};
