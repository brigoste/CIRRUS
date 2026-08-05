#include "simulation/SimulationRunner.hpp"
#include "simulation/Simulation.hpp"
#include "linear_system/Residual.hpp"
#include "postprocessing/BoundaryReconstructor.hpp"
#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"
#include "io/PlotUtils.hpp"

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
        throw std::runtime_error("CG not valid for advection-diffusion (non-symmetric system)");
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

    auto phi = sim.solve();

    const auto& mesh = sim.mesh();
    const auto& system = sim.system();

    std::cout << "\n================ SOLVER COMPLETE ================\n\n";

    std::vector<double> residual;

    {
        // Timer timer("Residual calculation");
        residual = computeResidual(system, phi);
    }

    PointField field;

    {
        // Timer timer("Boundary reconstruction");
        field = BoundaryReconstructor::reconstruct( mesh, sim.boundary(), sim.model(), phi);
    }
    // -----------------------------
    // Output paths
    // -----------------------------
    std::filesystem::create_directories(paths.outputRoot);

    auto csvPath  = paths.outputRoot / "solution.csv";
    auto jsonPath = paths.outputRoot / "solution.json";
    auto vtkPath  = paths.outputRoot / "solution.vtu";

    // -----------------------------
    // Write outputs
    // -----------------------------
    {
        // Timer timer("VTK output");
        VTKWriter::writeVTU(mesh, phi, vtkPath.string());
    }

    {
        // Timer timer("CSV output");
        FieldWriter::writeCSVDebug( field, system.RHS(), residual, csvPath.generic_string());
    }
    // -----------------------------
    // Plotting
    // -----------------------------
    if (cfg.io.plot_enabled) {
        // Timer time("Plotting");
        
        runPlot(paths, csvPath); 
    }
}
