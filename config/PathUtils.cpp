#include "config/PathUtils.hpp"

#include <cstdlib>
#include <iostream>

PathContext buildPaths(
    const SimulationConfig& cfg,
    const std::filesystem::path& projectRoot)
{
    PathContext p;

    p.projectRoot = std::filesystem::absolute(projectRoot);
    p.outputRoot = p.projectRoot / cfg.io.output_root;
    p.verificationRoot = p.outputRoot / "verification";
    p.scriptRoot = p.projectRoot / "scripts";
    p.pythonExecutable = cfg.io.python_executable;

    return p;
}
