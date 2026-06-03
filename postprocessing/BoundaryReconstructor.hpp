#pragma once

#include "postprocessing/Field1D.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/MeshGeometry.hpp"
#include "physics/PhysicsModel.hpp"

struct BoundaryField1D
{
    double x;
    double phi;
};

class BoundaryReconstructor
{
public:

    static Field1D reconstruct(
        const MeshBase& mesh,
        const BoundaryPatchSystem& boundary,
        const PhysicsModel& model,
        const std::vector<double>& phi);
};
