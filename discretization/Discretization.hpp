#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/Face.hpp"
#include "linear_system/LinearSystem.hpp"
#include "mesh/BoundaryPatchSystem.hpp"

// Generic finite-volume diffusion discretization
void discretize(
    const MeshBase& mesh,
    const BoundaryPatchSystem& bcSystem,
    LinearSystem& sys,
    double k);

