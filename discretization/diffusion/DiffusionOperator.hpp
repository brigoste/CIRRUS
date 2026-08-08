#pragma once

#include "mesh/MeshBase.hpp"
#include "physics/PhysicsModel.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "test/verification/VerificationCase.hpp"
#include "linear_system/LinearSystem.hpp"

#include "discretization/operators/Operator.hpp"

class DiffusionOperator : public Operator
{
public:

    void apply(
        const MeshBase& mesh,
        const PhysicsModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase = nullptr
    ) const;

    void assemble(
        const FluxAccumulator& flux,
        LinearSystem& sys
    ) const override;
};