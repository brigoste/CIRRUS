#include "PlotUtils.hpp"

#include <cstdlib>
#include <iostream>
#include <algorithm>

void runPlot(
    const PathContext& paths,
    const std::filesystem::path& csvFile)
{
    const auto plotScript = paths.scriptRoot / "Plot.py";

    std::string script = plotScript.string();
    std::string csv = csvFile.string();

#ifdef _WIN32
    // Use forward slashes for Python under the Windows/MSYS2 environment.
    std::replace(script.begin(), script.end(), '\\', '/');
    std::replace(csv.begin(), csv.end(), '\\', '/');
#endif

    // Paths are quoted in case there are spaces in the names
    const std::string cmd =
        paths.pythonExecutable.string() + " "
        "\"" + script + "\" "
        "\"" + csv + "\"";

    const int rc = std::system(cmd.c_str());

    if (rc != 0)
    {
        std::cerr
            << "WARNING: Plotting failed (return code "
            << rc
            << ")\n";
    }
}
