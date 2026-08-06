#pragma once

#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"
#include "test/verification/VerificationCaseFactory.hpp"
#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include <fstream>

class VerificationRunner
{
public:
    static VerificationCaseConfig loadVerificationCase(const std::filesystem::path& path);
    static SimulationConfig applyVerificationOverrides(const SimulationConfig& base, const VerificationCaseConfig& verif);
    static void run( const SimulationConfig& baseCfg, const VerificationSuite& suite, const PathContext& paths);
};
