#include "config/PathUtils.hpp"
#include "config/SimulationConfig.hpp"

namespace Paths
{

std::filesystem::path outputRoot(
    const SimulationConfig& cfg)
{
    return cfg.io.output_root;
}

std::filesystem::path verificationRoot(
    const SimulationConfig& cfg)
{
    return outputRoot(cfg)
         / cfg.verification.output.directory;
}

std::filesystem::path verificationCSV(
    const SimulationConfig& cfg,
    const std::string& caseName)
{
    return verificationRoot(cfg)
         / (caseName + ".csv");
}

std::filesystem::path verificationJSON(
    const SimulationConfig& cfg,
    const std::string& caseName)
{
    return verificationRoot(cfg)
         / (caseName + ".json");
}

}
