#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/MeshGeometry.hpp"
#include "physics/PhysicsModel.hpp"
#include "io/PointField.hpp"

class BoundaryReconstructor
{
public:

    static PointField reconstruct(
        const MeshBase& mesh,
        const BoundaryPatchSystem& boundary,
        const PhysicsModel& model,
        const std::vector<double>& phi);

};
