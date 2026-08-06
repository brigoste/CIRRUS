#include <iostream>
#include <memory>
#include <filesystem>
#include "simulation/SimulationRunner.hpp"
#include "test/verification/VerificationRunner.hpp"
#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"
#include "config/PathUtils.hpp"

#include <locale>   // for setlocale
#include <codecvt>  // for UTF-8 conversion (C++11/14/17)
#include <algorithm>
#include <cmath>

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";
        std::filesystem::path configPath = "cases/base.json"; // fallback

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "--config" && i + 1 < argc) 
            { 
                configPath = argv[++i]; 
            }
        }

        if (argc < 2)
        {
            throw std::runtime_error(
                "No configuration file provided.\n"
                "Usage: CIRRUS <config_file.json>"
            );
        }

        if (!std::filesystem::exists(configPath)) 
        { 
            throw std::runtime_error( "Configuration file not found: " + configPath.string() ); 
        }

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
