#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/MeshGeometry.hpp"
#include "physics/ScalarTransport/ScalarTransportModel.hpp"
#include "fields/ScalarField.hpp"
#include "io/PointField.hpp"

class BoundaryReconstructor
{
public:

    static PointField reconstruct(
        const MeshBase& mesh,
        const BoundaryPatchSystem& boundary,
        const ScalarTransportModel& model,
        const ScalarField& phi);

};
