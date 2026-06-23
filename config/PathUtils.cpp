#include "config/PathUtils.hpp"

PathContext buildPaths(const SimulationConfig& cfg)
{
    PathContext p;
    p.outputRoot = std::filesystem::absolute(cfg.io.output_root);
    p.verificationRoot = p.outputRoot / "verification";
    return p;
}
