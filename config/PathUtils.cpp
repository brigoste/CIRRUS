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

    std::cout << "PATH DEBUG:\n"
          << "  projectRoot = " << p.projectRoot << '\n'
          << "  config output_root = " << cfg.io.output_root << '\n'
          << "  outputRoot = " << p.outputRoot << '\n';

    p.verificationRoot = p.outputRoot / "verification";

    p.scriptRoot = p.projectRoot / "scripts";

    if (const char* python = std::getenv("PYTHON_EXECUTABLE"))
    {
        p.pythonExecutable = python;
    }
    else
    {
        p.pythonExecutable = "python3";
    } 

    const std::string check = p.pythonExecutable.string() + " --version > /dev/null 2>&1";

    if (std::system(check.c_str()) != 0)
    {
        std::cerr
            << "WARNING: Python executable not available: "
            << p.pythonExecutable
            << '\n';
    }

    return p;
}
