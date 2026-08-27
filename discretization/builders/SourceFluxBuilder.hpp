#pragma once

#include "discretization/FluxAccumulator.hpp"

class MeshBase;
class ScalarTransportModel;
class VerificationCase;

class SourceFluxBuilder
{
public:
    void apply(
        const MeshBase& mesh,
        const ScalarTransportModel& model,
        FluxAccumulator& flux,
        const VerificationCase* verificationCase
    ) const;
};
