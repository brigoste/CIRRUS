#include <iostream>
#include <memory>
#include <filesystem>

#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"
#include "postprocessing/DerivedFields.hpp"
#include "postprocessing/ErrorNorms.hpp"

#include "Solver/SolverMethod.hpp"

#include "linear_system/Residual.hpp"

#include "simulation/Simulation.hpp"

// JSON includes
#include "config/SimulationConfig.hpp"
#include "tests/verification/VerificationRunner.hpp"

#include <locale>   // for setlocale
#include <codecvt>  // for UTF-8 conversion (C++11/14/17)
#include <algorithm>
#include <cmath>

// ==================== GLOBAL PATHS ====================

constexpr const char* PYTHON_EXE =
    "C:/Users/E40112856/.julia/conda/3/x86_64/python.exe";

constexpr const char* PLOT_SCRIPT =
    "C:/Users/E40112856/Packages/CIRRUS/scripts/Plot.py";

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

int main()
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        // -------------------------------------------------
        // 1. Load configuration
        // -------------------------------------------------
        const bool useJSON = true;

        SimulationConfig cfg;

        if (useJSON)
        {
            std::cout << "Loading JSON config...\n";
            cfg = loadConfig("C:/Users/E40112856/Packages/CIRRUS/output/verification/Linear1D.json");
        }
        else
        {
            std::cout << "Using default config...\n";
            cfg = defaultConfig();
        }        

        // -------------------------------------------------
        // 2. Initialize simulation with loaded variables
        // -------------------------------------------------
        Simulation sim(cfg);
        sim.assemble();

        // -------------------------------------------------
        // 3. Run Simulation
        // -------------------------------------------------

        std::cout << "# of cells = " << sim.mesh().ncells()
                  << "\n# of faces = " << sim.mesh().nfaces() << "\n";

        std::vector<double> phi = sim.solve();

        const auto& mesh = sim.mesh();
        const auto& system = sim.system();

        // -------------------------------------------------
        // 4. Verification (optional diagnostic layer)
        // -------------------------------------------------
        // if (cfg.verification.enabled)
        // {
        //     auto verificationCase =
        //         VerificationRegistry::instance().create(
        //             cfg.verification);

        //     VerificationRunner::run(
        //         sim.mesh(),
        //         phi,
        //         sim.system(),
        //         *verificationCase,
        //         cfg.verification);
        // }
        if (cfg.verification.enabled)
        {
            const std::string& folder = "verification/";
            VerificationRunner::run(cfg);
        }

        // -------------------------------------------------
        // 5. Solver summary
        // -------------------------------------------------
        std::cout << "\n================ SOLVER COMPLETE ================\n\n";

        // auto [minIt, maxIt] =
        //     std::minmax_element(phi.begin(), phi.end());

        // std::cout << "Interior cells = " << mesh.ncells()
        //           << ", nodes = " << mesh.nnodes() << '\n';

        // std::cout << "Min Value: " << *minIt << "\n";
        // std::cout << "Max Value: " << *maxIt << "\n";

        auto residual = computeResidual(system, phi);

        double maxAbsResidual = 0.0;
        for (double r : residual)
            maxAbsResidual = std::max(maxAbsResidual, std::abs(r));

        // std::cout << "Max |Residual|: " << maxAbsResidual << "\n\n";

        // -------------------------------------------------
        // 6. Output (VTK + CSV)
        // -------------------------------------------------
        VTKWriter::writeVTU(
            mesh,
            phi,
            "../output/solution.vtu");

        auto field = BoundaryReconstructor::reconstruct(
            mesh,
            sim.boundary(),
            sim.model(),
            phi);
        
        // Sort 1D field values by x-position
        if(field.dim == 1){
            std::vector<std::size_t> idx(field.size());

            std::iota(idx.begin(), idx.end(), 0);

            std::sort(
                idx.begin(),
                idx.end(),
                [&](std::size_t a, std::size_t b)
                {
                    return field.x[a].x[0] < field.x[b].x[0];
                });

            // Build sorted copies
            PointField sortedField;
            sortedField.reserve(field.size());

            for (std::size_t i : idx)
            {
                sortedField.push_back(
                    field.x[i],
                    field.phi[i]);
            }

            // Replace original field
            field = std::move(sortedField);
        }
        
        for(std::size_t i = 0; i < field.size(); ++i){
            std::cout << "Cell " << i << ", T = " << field.phi.at(i) << "\n"; 
        }

        for (std::size_t i = 0; i < 3; ++i)
        {
            std::cout << "\nROW " << i << "\n";

            for (std::size_t j = 0; j < system.size(); ++j)
            {
                double a = system.coeff(i,j);

                if (std::abs(a) > 1e-12)
                {
                    std::cout
                        << "A[" << i << "," << j << "] = "
                        << a << "\n";
                }
            }

            std::cout
                << "rhs = "
                << system.rhs(i)
                << "\n";
        }


        std::string output_csv_filepath; 

        if(cfg.verification.enabled){ output_csv_filepath = "../output/solution.csv"; }
        else{ output_csv_filepath = cfg.verification.output.directory; }

        

        FieldWriter::writeCSVDebug(
            field,
            system.RHS(),
            residual,
            output_csv_filepath);

        // -------------------------------------------------
        // 7. Post-processing (plotting)
        // -------------------------------------------------
        constexpr bool plot_solution_field = true;

        if (plot_solution_field)
        {
            std::cout << "=========== PLOTTING SOLUTION ==============\n\n";
            runPlot(output_csv_filepath);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "EXCEPTION: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "UNKNOWN EXCEPTION\n";
    }

    return 0;
}
