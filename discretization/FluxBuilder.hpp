#pragma once
#include "discretization/FluxAccumulator.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "physics/PhysicsModel.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "tests/verification/VerificationCase.hpp"

class FluxBuilder
{
public:
    static void buildFlux(
        const MeshBase& mesh,
        const PhysicsModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase);
};
