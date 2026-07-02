#include "PlotUtils.hpp"
#include <iostream>
#include <cstdlib>

constexpr const char* PYTHON_EXE = "C:/Users/E40112856/.julia/conda/3/x86_64/python.exe";

constexpr const char* PLOT_SCRIPT = "C:/Users/E40112856/Packages/CIRRUS/scripts/Plot.py";

void runPlot(const std::string& csvFile)
{
    std::string cmd =
        "cmd /c "
        "\"\"" + std::string(PYTHON_EXE) +
        "\" \"" + PLOT_SCRIPT +
        "\" \"" + csvFile +
        "\"\"";

    std::cout << cmd << '\n';       // forces a command prompt run of the python script

    int rc = std::system(cmd.c_str());

    std::cout << "\nReturn code = " << rc << "\n\n";
}
