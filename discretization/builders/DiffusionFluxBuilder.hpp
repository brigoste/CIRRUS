#pragma once

#include "discretization/FluxAccumulator.hpp"

class MeshBase;
class ScalarTransportModel;
class BoundaryPatchSystem;
class FluxAccumulator;
class VerificationCase;

class DiffusionFluxBuilder
{
public:
    void apply(
        const MeshBase& mesh,
        const ScalarTransportModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase = nullptr
    ) const;
};
