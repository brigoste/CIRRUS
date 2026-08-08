#pragma once

#include "discretization/FluxAccumulator.hpp"
#include "discretization/DiffusionFluxBuilder.hpp"
#include "discretization/ConvectionFluxBuilder.hpp"
#include "discretization/SourceFluxBuilder.hpp"

class MeshBase;
class PhysicsModel;
class BoundaryPatchSystem;
class VerificationCase;

class FluxBuilder
{
public:

    void buildFlux(
        const MeshBase& mesh,
        const PhysicsModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase = nullptr
    ) const;

private:

    DiffusionFluxBuilder diffusionFlux_;
    ConvectionFluxBuilder convectionFlux_;
    SourceFluxBuilder sourceFlux_;
};