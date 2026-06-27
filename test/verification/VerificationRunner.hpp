#pragma once

#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"
#include "tests/verification/VerificationCaseFactory.hpp"
#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"

class VerificationRunner
{
public:
    static void run(const SimulationConfig& cfg, const PathContext& paths);
};
