#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>

#include "simulation/SimulationRunner.hpp"
#include "test/verification/VerificationRunner.hpp"

#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"
#include "config/PathUtils.hpp"

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        std::filesystem::path configPath = "cases/base.json";

        // -------------------------------------------------
        // Command line arguments
        // -------------------------------------------------

        bool plotOverride = false;

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "--config" && i + 1 < argc)
            {
                configPath = argv[++i];
            }
            else if (arg == "--plot")
            {
                plotOverride = true;
            }
        }

        if (argc < 2)
        {
            throw std::runtime_error(
                "No configuration file provided.\n"
                "Usage: CIRRUS --config <config_file.json>"
            );
        }

        if (!std::filesystem::exists(configPath))
        {
            throw std::runtime_error(
                "Configuration file not found: "
                + configPath.string()
            );
        }        

        const std::filesystem::path projectRoot = std::filesystem::current_path();

        // -------------------------------------------------
        // Verification mode
        // -------------------------------------------------

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

            if (plotOverride) { suite.plot_enabled = true; }

            PathContext paths = buildPaths(cfg, projectRoot);            

            VerificationRunner::run(
                cfg,
                suite,
                paths
            );

            std::cout << "\n================ VERIFICATION COMPLETE ================\n";


            return 0;
        }

        // -------------------------------------------------
        // Normal simulation mode
        // -------------------------------------------------
        SimulationConfig cfg = loadConfig(configPath);
        
        if (plotOverride) { cfg.io.plot_enabled = true; }
        
        PathContext paths = buildPaths(cfg, projectRoot);

        std::cout << "\n================ USER SIMULATION MODE ================\n";

        SimulationRunner::run(
            cfg,
            paths
        );

        std::cout << "\n================ SIMULATION COMPLETE ================\n";
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "EXCEPTION: "
            << e.what()
            << "\n";

        return 1;
    }

    return 0;
}