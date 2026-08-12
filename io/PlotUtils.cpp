#include "PlotUtils.hpp"

#include <cstdlib>
#include <iostream>

void runPlot(
const PathContext& paths,
const std::filesystem::path& csvFile)
{
    const auto plotScript = paths.scriptRoot / "Plot.py";

    std::string cmd =
        "\"" + paths.pythonExecutable.string() + "\" "
        + "\"" + plotScript.string() + "\" "
        + "\"" + csvFile.string() + "\"";

    int rc = std::system(cmd.c_str());

    std::cout << "\nReturn code = "
            << rc
            << "\n\n";

}