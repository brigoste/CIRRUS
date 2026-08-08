#pragma once
#include "discretization/FluxAccumulator.hpp"
#include "discretization/diffusion/DiffusionOperator.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "physics/PhysicsModel.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "test/verification/VerificationCase.hpp"

class FluxBuilder
{
public:
    FluxBuilder(const DiffusionOperator& diffusion)
        :
        diffusion_(diffusion)
    {}
    void buildFlux(
        const MeshBase& mesh,
        const PhysicsModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase);
private:
        const DiffusionOperator& diffusion_;
};
