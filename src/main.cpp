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

//std::filesystem::path resolveOutputPath( const std::filesystem::path& root, const std::string& relative) { return root / relative; }

int main()
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        const std::filesystem::path configPath = "C:/Users/E40112856/Packages/CIRRUS/cases/verification/verification_suite.json";


        if (configPath.filename() == "verification_suite.json")
        {
            std::cout << "Loading verification suite: " << configPath << "\n";

            VerificationSuite suite = loadVerificationSuite(configPath);
         
            suite.case_directory = configPath.parent_path().string();

            std::cout << "Verification cases loaded:\n";

            for (const auto& c : suite.cases)
            {
                std::cout << "  case: " << c.name << "\n";
            }


            /*
             * Load any minimal config needed for paths.
             *
             * This is NOT the simulation config.
             * Individual cases will load:
             *
             * Linear1D.json
             * Quadratic1D.json
             * ...
             *
             * and resolve their own extends.
             */
            SimulationConfig cfg;

            cfg.io.output_root = "C:/Users/E40112856/Packages/CIRRUS/output";

            PathContext paths = buildPaths(cfg);

            // std::cout << "\nVerification output:\n";
            // std::cout << "  verification root: "
            //           << paths.verificationRoot
            //           << "\n";

            // std::cout << "  output root: "
            //           << paths.outputRoot
            //           << "\n";


            VerificationRunner::run( cfg, suite, paths );

            std::cout << "\n================ VERIFICATION COMPLETE ================\n";

            return 0;
        }

        // -----------------------------
        // Normal simulation mode
        // -----------------------------

        SimulationConfig cfg = loadConfig(configPath);

        PathContext paths = buildPaths(cfg);


        std::cout << "\n================ USER SIMULATION MODE ================\n";

        SimulationRunner::run( cfg, paths );

        std::cout << "\n================ SIMULATION COMPLETE ================\n";
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "EXCEPTION: "
            << e.what()
            << "\n";
    }

    return 0;
}
