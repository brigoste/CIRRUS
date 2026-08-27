#pragma once

#include "discretization/FluxAccumulator.hpp"
#include "discretization/builders/DiffusionFluxBuilder.hpp"
#include "discretization/builders/ConvectionFluxBuilder.hpp"
#include "discretization/builders/SourceFluxBuilder.hpp"

class MeshBase;
class ScalarTransportModel;
class BoundaryPatchSystem;
class VerificationCase;

class FluxBuilder
{
public:

    void buildFlux(
        const MeshBase& mesh,
        const ScalarTransportModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase = nullptr
    ) const;

private:

    DiffusionFluxBuilder diffusionFlux_;
    ConvectionFluxBuilder convectionFlux_;
    SourceFluxBuilder sourceFlux_;
};