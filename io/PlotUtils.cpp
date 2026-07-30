#include "PlotUtils.hpp"

#include <iostream>
#include <cstdlib>


void runPlot(
    const PathContext& paths,
    const std::filesystem::path& csvFile
)
{
    const std::filesystem::path pythonExecutable = "C:/Users/E40112856/.julia/conda/3/x86_64/python.exe";

    const auto plotScript = paths.scriptRoot / "Plot.py";

    std::string cmd =
        "cmd /c "
        "\"\"" + pythonExecutable.string() +
        "\" \"" + plotScript.string() +
        "\" \"" + csvFile.string() +
        "\"\"";

    std::cout << cmd << '\n';

    int rc = std::system(cmd.c_str());

    std::cout << "\nReturn code = "
              << rc
              << "\n\n";
}
