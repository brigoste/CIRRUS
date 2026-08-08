#pragma once

// #include "mesh/MeshBase.hpp"
// #include "physics/PhysicsModel.hpp"
#include "discretization/FluxAccumulator.hpp"

class MeshBase;
class PhysicsModel;
class VerificationCase;

class SourceFluxBuilder
{
public:
    void apply(
        const MeshBase& mesh,
        const PhysicsModel& model,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase
    ) const;
};