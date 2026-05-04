#include <memory>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <windows.h>
#include "system/HeatSystem1D.hpp"
#include "bc/DirichletBC.hpp"
#include "bc/ConvectiveBC.hpp"
#include "bc/NeumannBC.hpp"

#ifdef _WIN32

bool runPythonProcess(const std::wstring& pythonExe,
                      const std::wstring& script,
                      const std::wstring& arg)
{
    std::wstring cmd =
        L"\"" + pythonExe + L"\" \"" +
        script + L"\" \"" +
        arg + L"\"";

    STARTUPINFOW si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};

    std::vector<wchar_t> buffer(cmd.begin(), cmd.end());
    buffer.push_back(L'\0');

    BOOL ok = CreateProcessW(
        nullptr,
        buffer.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!ok) {
        std::wcerr << L"CreateProcess failed: " << GetLastError() << L"\n";
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

#endif

// To run in terminal, go to root directory (CIRRUS) and run build_and_run.bat

namespace fs = std::filesystem;

inline std::string runPython(const std::string& script,
                             const std::string& arg)
{
    return "\"" + std::string(PYTHON_EXECUTABLE) + "\" \"" + script + "\" \"" + arg + "\"";
}

int main() {
    std::cout << "Program started\n";

    std::cout << "================== INTIALIZING PROBLEM SPACE =========================" << std::endl;

    int n = 15;
    double L = 5.0;
    double A = 1.0;
    double k = 100.0;

    double h = 10.0;
    double T_inf = 200.0;

    // ADD VARIABLE AREA for non-1D scenarios

    HeatSystem1D system(n, L, A, k);

    auto& c = system.coeffs();   // correct

    system.setSource(
        [](double x) { return -0.0; },   // Su
        [](double x) { return -0.0; }       // Sp
    );

    // Left boundary (node 0)
    system.addBC(std::make_unique<DirichletBC>(0, 100.0));

    // Middle boundary (for fun?)
    // system.addBC(std::make_unique<DirichletBC>(n/2, -5));

    // Right boundary (node n-1 = 19)
    // system.addBC(std::make_unique<ConvectiveBC>(n-1, h, T_inf));
    // system.addBC(std::make_unique<NeumannBC>(n-1, 1.0e4));
    system.addBC(std::make_unique<DirichletBC>(n-1,200.0));
    
    system.assemble();

    std::cout << "======================== RUNNING SOLVER ===============================" << std::endl;
    auto T = system.solve();

    for (auto Ti : T) {
        std::cout << Ti << "\n";
    }
    std::cout << "T_0 = " << T[0] << std::endl;
    std::cout << "T_f = " << T[-1] << std::endl;
    const auto& mesh = system.mesh();

    std::cout << "=================== SAVING OUTPUT TO EXTERNAL FILE ===================" << std::endl;
    auto root = std::filesystem::current_path().parent_path();
    auto outDir = root / "output";
    std::filesystem::create_directories(outDir);
    // std::filesystem::create_directories("../output");
    auto outFile = outDir / "solution.csv";
    std::ofstream file(outFile);

    file << "x,T\n";
    for (int i = 0; i < mesh.n; ++i) {
        file << mesh.x[i] << "," << T[i] << "\n";
    }
    file.close();

    // ---- Python call (clean + deterministic) ----
    //std::filesystem::path root = std::filesystem::current_path().parent_path();
    // std::filesystem::path script = std::filesystem::absolute("plot.py");
    // std::filesystem::path data   = std::filesystem::absolute("output/solution.csv");

    std::filesystem::path pythonExe = PYTHON_EXECUTABLE;
    std::filesystem::path script    = root / "scripts" / "Plot.py";
    std::filesystem::path data      = outDir / "solution.csv";

    std::cout << "Data saved to: " << data << std::endl;

    std::cout << "=================== Plotting Data ===================" << std::endl;
    runPythonProcess(
        pythonExe.wstring(),
        script.wstring(),
        std::filesystem::absolute(data).wstring()
    );

    return 0;
}
