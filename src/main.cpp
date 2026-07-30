#include <iostream>
#include <memory>
#include <filesystem>
#include "simulation/SimulationRunner.hpp"
#include "tests/verification/VerificationRunner.hpp"
#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"
#include "config/PathUtils.hpp"

#include <locale>   // for setlocale
#include <codecvt>  // for UTF-8 conversion (C++11/14/17)
#include <algorithm>
#include <cmath>

int main()
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        const std::filesystem::path configPath = "C:/Users/E40112856/Packages/CIRRUS/cases/verification/verification_suite.json";

        // verification_suite.json:
        // CIRRUS/cases/verification/verification_suite.json
        // Therefore project root is three directories above.

        const std::filesystem::path projectRoot = configPath.parent_path().parent_path().parent_path();


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
            SimulationConfig cfg;

            PathContext paths = buildPaths(cfg, projectRoot);

            VerificationRunner::run( cfg, suite, paths );

            std::cout << "\n================ VERIFICATION COMPLETE ================\n";

            return 0;
        }

        // -----------------------------
        // Normal simulation mode
        // -----------------------------

        SimulationConfig cfg = loadConfig(configPath);
        cfg.io.output_root = "output";

        PathContext paths = buildPaths(cfg, projectRoot);

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
