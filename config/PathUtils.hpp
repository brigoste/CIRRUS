#pragma once

#include <filesystem>
#include <string>

class SimulationConfig;

namespace Paths
{
    std::filesystem::path outputRoot(
        const SimulationConfig& cfg);

    std::filesystem::path verificationRoot(
        const SimulationConfig& cfg);

    std::filesystem::path verificationCSV(
        const SimulationConfig& cfg,
        const std::string& caseName);

    std::filesystem::path verificationJSON(
        const SimulationConfig& cfg,
        const std::string& caseName);
}
