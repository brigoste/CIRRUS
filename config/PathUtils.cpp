#include "config/PathUtils.hpp"

PathContext buildPaths(
    const SimulationConfig& cfg,
    const std::filesystem::path& projectRoot)
{
    PathContext p;

    p.projectRoot = std::filesystem::absolute(projectRoot);

    p.outputRoot = p.projectRoot / cfg.io.output_root;

    p.verificationRoot = p.outputRoot / "verification";

    p.scriptRoot = p.projectRoot / "scripts";

    return p;
}
