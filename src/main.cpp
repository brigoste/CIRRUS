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
    STARTUPINFOW si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};

    // IMPORTANT: command line does NOT include executable
    std::wstring cmdLine =
        L"\"" + script + L"\" \"" + arg + L"\"";

    std::wstring mutableCmd = cmdLine;

    BOOL success = CreateProcessW(
        pythonExe.c_str(),   // executable
        mutableCmd.data(),   // arguments only
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!success) {
        std::wcerr << L"CreateProcess failed. Error: " << GetLastError() << L"\n";
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

#endif

// To run in terminal, go to root directory (HT_CFD_PACKAGE) and run build_and_run.bat

namespace fs = std::filesystem;

inline std::string runPython(const std::string& script,
                             const std::string& arg)
{
    return "\"" + std::string(PYTHON_EXECUTABLE) + "\" \"" + script + "\" \"" + arg + "\"";
}

int main() {

    int n = 50;
    double L = 5.0;
    double A = 1.0;
    double k = 100.0;

    double h = 1000.0;
    double T_inf = 400.0;

    HeatSystem1D system(n, L, A, k);

    auto& c = system.coeffs();   // correct

    system.setSource(
        [](double x) { return 1000.0; },   // Su
        [](double x) { return 0.0; }       // Sp
    );
    // // example: no Sp, uniform constant volumetric source
    // for (int i = 0; i < c.Su.size(); ++i) {
    //     c.Su[i] = 1000.0;   // source strength per cell
    //     c.Sp[i] = 0.0;      // no linear term
    // }

    // Left boundary (node 0)
    system.addBC(std::make_unique<DirichletBC>(0, 100.0));

    // Right boundary (node n-1 = 19)
    system.addBC(std::make_unique<ConvectiveBC>(n-1, h, T_inf));
    // system.addBC(std::make_unique<DirichletBC>(n-1,0.0));
    

    system.assemble();

    auto T = system.solve();

    for (auto Ti : T) {
        std::cout << Ti << "\n";
    }
    const auto& mesh = system.mesh();

    std::filesystem::create_directories("output");
    std::ofstream file("output/solution.csv");

    file << "x,T\n";
    for (int i = 0; i < mesh.n; ++i) {
        file << mesh.x[i] << "," << T[i] << "\n";
    }
    file.close();

    // ---- Python call (clean + deterministic) ----
    //std::filesystem::path root = std::filesystem::current_path().parent_path();
    // std::filesystem::path script = std::filesystem::absolute("plot.py");
    // std::filesystem::path data   = std::filesystem::absolute("output/solution.csv");
    auto root = std::filesystem::current_path();

    std::filesystem::path pythonExe = PYTHON_EXECUTABLE;
    std::filesystem::path script    = std::filesystem::current_path().parent_path() / "plot.py";
    std::filesystem::path data      = std::filesystem::current_path().parent_path() / "output" / "solution.csv";

    runPythonProcess(
        pythonExe.wstring(),
        script.wstring(),
        data.wstring()
    );

    // std::string cmd =
    //     "\"" + std::string(PYTHON_EXECUTABLE) + "\" "
    //     "\"" + script.string() + "\" "
    //     "\"" + data.string() + "\"";
    // std::cout << "CMD: " << cmd << "\n";

    // std::wstring cmd =
    //     std::wstring(L"\"") + std::wstring(PYTHON_EXECUTABLE, PYTHON_EXECUTABLE + strlen(PYTHON_EXECUTABLE)) + L"\" "
    //     L"\"" + script.wstring() + L"\" "
    //     L"\"" + data.wstring() + L"\"";

    // _wsystem(cmd.c_str());

    // int ret = std::system(cmd.c_str());
    // std::cout << "Return code: " << ret << "\n";

    // std::filesystem::path root = std::filesystem::current_path().parent_path();
    // std::filesystem::path pythonExe = PYTHON_EXECUTABLE;
    // std::filesystem::path script = root / "plot.py";
    // std::filesystem::path data = root / "output" / "solution.csv";

    // std::cout << script << "\n";
    // std::cout << data << "\n";
    // std::cout << pythonExe << "\n";

    // std::string cmd =
    //     "\"" + pythonExe.string() + "\" " +
    //     "\"" + script.string() + "\" " +
    //     "\"" + data.string() + "\"";

    // int ret = std::system(cmd.c_str());
    // std::cout << "Python return code: " << ret << "\n";

    return 0;
}
