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

    if (const char* python = std::getenv("PYTHON_EXECUTABLE"))
    {
        p.pythonExecutable = python;
    }
    else
    {
        p.pythonExecutable = "python";
    } 

    return p;
}
