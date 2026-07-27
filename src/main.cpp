#include <iostream>
#include <memory>
#include <filesystem>

#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"
#include "postprocessing/DerivedFields.hpp"
#include "postprocessing/ErrorNorms.hpp"

#include "solver/SolverMethod.hpp"

#include "linear_system/Residual.hpp"

#include "simulation/Simulation.hpp"
#include "simulation/SimulationRunner.hpp"

// JSON includes
#include "config/SimulationConfig.hpp"
#include "tests/verification/VerificationRunner.hpp"
#include "config/PathContext.hpp"
#include "config/PathUtils.hpp"

// Plot Setups
#include "io/PlotUtils.hpp"

#include <locale>   // for setlocale
#include <codecvt>  // for UTF-8 conversion (C++11/14/17)
#include <algorithm>
#include <cmath>

std::filesystem::path resolveOutputPath( const std::filesystem::path& root, const std::string& relative) { return root / relative; }

int main()
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        const std::string caseFile = "C:/Users/E40112856/Packages/CIRRUS/cases/user/Steady_1D_heat.json";

        SimulationConfig cfg = loadConfig(caseFile);

        // -------------------------------------------------
        // BUILD PATH CONTEXT (CRITICAL FIX)
        // -------------------------------------------------
        PathContext paths = buildPaths(cfg);
        std::cout << paths.verificationRoot << std::endl;
        std::cout << paths.outputRoot << std::endl;

        // -------------------------------------------------
        // Single dispatch point
        // -------------------------------------------------

        if (cfg.verificationSuite.enabled) 
        {
            std::cout << "\n================ VERIFICATION MODE ================\n";

            VerificationRunner::run(cfg, paths);

            std::cout << "\n================ VERIFICATION COMPLETE ================\n";
            return 0;
        }

        std::cout << "\n================ USER SIMULATION MODE ================\n";

        SimulationRunner::run(cfg, paths);

        std::cout << "\n================ SIMULATION COMPLETE ================\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "EXCEPTION: " << e.what() << "\n";
    }

    return 0;
}
