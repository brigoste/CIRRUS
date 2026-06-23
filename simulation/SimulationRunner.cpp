#include "simulation/SimulationRunner.hpp"

#include "simulation/Simulation.hpp"

#include "linear_system/Residual.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"

#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"
#include "io/PlotUtils.hpp"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

void SimulationRunner::run(
    const SimulationConfig& cfg,
    const PathContext& paths)
{
    std::cout << "\n================ USER SIMULATION ================\n";

    Simulation sim(cfg);
    sim.assemble();

    std::cout << "# of cells = " << sim.mesh().ncells()
              << "\n# of faces = " << sim.mesh().nfaces()
              << "\n";

    auto phi = sim.solve();

    const auto& mesh = sim.mesh();
    const auto& system = sim.system();

    std::cout << "\n================ SOLVER COMPLETE ================\n\n";

    auto residual = computeResidual(system, phi);

    PointField field =
        BoundaryReconstructor::reconstruct(
            mesh,
            sim.boundary(),
            sim.model(),
            phi);

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
    VTKWriter::writeVTU(mesh, phi, vtkPath.string());

    FieldWriter::writeCSVDebug(
        field,
        system.RHS(),
        residual,
        csvPath.generic_string());

    // -----------------------------
    // Plotting
    // -----------------------------
    if (cfg.io.plot_enabled)
    {
        runPlot(csvPath.generic_string());
    }
}
