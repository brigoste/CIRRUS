#include "simulation/SimulationRunner.hpp"
#include "simulation/Simulation.hpp"
#include "equation_systems/Residual.hpp"
#include "postprocessing/BoundaryReconstructor.hpp"
#include "io/OutputManager.hpp"
#include "io/OutputData.hpp"
#include "io/PlotUtils.hpp"
#include "io/OutputBuilder.hpp"
#include "fields/FieldNames.hpp"

#include "utils/Timer.hpp"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

void SimulationRunner::validate(
    const SimulationConfig& cfg
)
{
    // -------------------------------------------------
    // Exception handling for illadvised solver pairing
    // -------------------------------------------------
    if (std::string(physics::to_string(cfg.physics.type)) == "advection-diffusion" && cfg.solver.method == solver::Method::CG)
    {
        throw std::runtime_error("CG not valid for advection-diffusion (non-symmetric system)");
    }
}

void SimulationRunner::run(
    const SimulationConfig& cfg,
    const PathContext& paths)
{
    // Timer totalTimer("SimulationRunner::run");
    
    std::cout << "\n================ USER SIMULATION ================\n";

    {
        // Timer timer("Validation");
        validate(cfg);      // Makes sure solver/model pairings are ok.
    }

    Simulation sim(cfg);

    {
        // Timer timer("Assembly");
        sim.assemble();
    }

    std::cout << "# of cells = " << sim.mesh().ncells()
              << "\n# of faces = " << sim.mesh().nfaces()
              << "\n";

    sim.solve();
    const auto& temperature = sim.fields().scalar(FieldName::Temperature);
    const auto& mesh = sim.mesh();
    const auto& system = sim.system();

    std::cout << "\n================ SOLVER COMPLETE ================\n\n";

    std::vector<double> residual;

    {
        // Timer timer("Residual calculation");
        residual = computeResidual(system, temperature);
    }

    PointField field;

    {
        // Timer timer("Boundary reconstruction");
        field = BoundaryReconstructor::reconstruct( mesh, sim.boundary(), sim.model(), temperature);
    }

    // -----------------------------
    // Output paths
    // -----------------------------
    std::filesystem::create_directories(paths.outputRoot);

    // -----------------------------
    // Organize outputs
    // -----------------------------
    auto output =
        OutputBuilder::build(
            sim,
            temperature,
            residual);
    
    // -----------------------------
    // Write outputs
    // -----------------------------
    {
        // Timer timer("Output Writing");
        OutputManager::write(
            output,
            paths.outputRoot);
    }

    // -----------------------------
    // Plotting
    // -----------------------------
    if (cfg.io.plot_enabled) {
        // Timer time("Plotting");

        auto csvPath = paths.outputRoot / "solution.csv";
        
        runPlot(paths, csvPath); 
    }
}