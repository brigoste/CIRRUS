#pragma once

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"

// Generic finite-volume diffusion discretization
void discretize(
    const MeshBase& mesh,
    LinearSystem& sys,
    double k
);

