#include "PlotUtils.hpp"

#include <iostream>
#include <cstdlib>


void runPlot(
    const PathContext& paths,
    const std::filesystem::path& csvFile)
{
    const auto plotScript = paths.scriptRoot / "Plot.py";

    std::string cmd =
        "cmd /c "
        + paths.pythonExecutable.string()
        + " "
        + plotScript.string()
        + " "
        + csvFile.string();

    // std::cout << cmd << '\n';            // Show commandline output in terminal (not necessary to execute command)

    int rc = std::system(cmd.c_str());

    std::cout << "\nReturn code = "
              << rc
              << "\n\n";
}
