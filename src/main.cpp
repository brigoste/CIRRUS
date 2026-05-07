#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <functional>
#include <cmath>

#include "system/HeatSystem1D.hpp"
#include "Solver/SolverMethod.hpp"
#include "utils/StringConvert.hpp"
#include "bc/DirichletBC.hpp"
#include "bc/ConvectiveBC.hpp"
#include "bc/NeumannBC.hpp"

#include "config.h"


#ifdef _WIN32
#include <windows.h>
#endif

// ------------------------------------------------------------
// Python runner 
// ------------------------------------------------------------
#ifdef _WIN32
bool runPythonProcess(
    const std::string& pythonExe,
    const std::string& script,
    const std::string& data,
    const std::string& solver)
{
    std::wstring wPython = to_wstring(pythonExe);
    std::wstring wScript = to_wstring(script);
    std::wstring wData   = to_wstring(data);
    std::wstring wSolver = to_wstring(solver);

    std::wstring cmd =
        L"\"" + wPython + L"\" \"" +
        wScript + L"\" \"" +
        wData + L"\" \"" +
        wSolver + L"\"";

    STARTUPINFOW si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};

    // std::wcout << L"CMD: " << cmd << L"\n";
    // std::wcout << L"PY: " << wPython << L"\n";

    BOOL ok = CreateProcessW(
        nullptr,
        cmd.data(),
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

namespace fs = std::filesystem;

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main()
{
    std::cout << "================ INITIALIZING SYSTEM ================\n";

    // -------------------------
    // Problem setup
    // -------------------------
    const int n = 1500;
    const double L = 500.0;
    const double A = 3.0;
    const double k = 100.0;
    
    bool output = false;        // to see iteration output for GS and SOR

    // Valid setups, output is an overloaded variable with parameters defaulted to true.
    // HeatSystem1D system(n, L, A, k);

    // -------------------------
    // Instantiate System
    // -------------------------
    HeatSystem1D system(n, L, A, k, output);

    // -------------------------
    // Source terms
    // -------------------------
    system.setSource(
        [](double x) { return 0.0*x; },
        [](double x) { return 0.0*x; }
    );

    // -------------------------
    // Boundary conditions
    // -------------------------
    system.addBC(std::make_unique<DirichletBC>(0, 300.0));
    system.addBC(std::make_unique<DirichletBC>(n - 1, 400.0));
    // system.addBC(std::make_unique<NeumannBC>(n-1, 100));
    // system.addBC(std::make_unique<ConvectiveBC>(n-1,150,273.15));

    // ConvectiveBC doesn't seem to work.

    // -------------------------
    // Assemble system
    // -------------------------
    system.assemble();

    std::cout << "================ SOLVING ================\n";

    // -------------------------
    // Solver selection (TDMA, GS, SOR)
    // -------------------------
    SolverMethod method = SolverMethod::SOR;

    int iter = 1250000;
    double tol = 5e-6;
    double omega = 1.2;

    // double omega = 1/(1+sin(3.141926/n)); //"Optimal" (?) omega

    std::vector<double> T;

    T = system.solve(method, iter, tol, omega);

    // -------------------------
    // Output
    // -------------------------
    const auto& mesh = system.mesh();

    fs::path outDir = "../output";
    fs::create_directories(outDir);

    std::ofstream file(outDir / "solution.csv");

    file << "# solver=" << to_string(method) << "\n";
    file << "x,T\n";

    for (int i = 0; i < n; ++i)
    {
        file << mesh.x[i] << "," << T[i] << "\n";
    }

    file.close();

    std::cout << "Solution written.\n";

    // -------------------------
    // Python visualization
    // -------------------------
    fs::path pythonExe = PYTHON_EXECUTABLE;
    fs::path script    = fs::current_path().parent_path() / "scripts" / "Plot.py";
    fs::path data      = fs::current_path().parent_path() / "output" / "solution.csv";

    runPythonProcess(
        pythonExe.string(),
        script.string(),
        data.string(),
        to_string(method)
    );

    return 0;
}
