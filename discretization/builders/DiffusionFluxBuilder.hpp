#pragma once

#include "discretization/FluxAccumulator.hpp"

class MeshBase;
class PhysicsModel;
class BoundaryPatchSystem;
class FluxAccumulator;
class VerificationCase;

class DiffusionFluxBuilder
{
public:
    void apply(
        const MeshBase& mesh,
        const PhysicsModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase = nullptr
    ) const;
};