#include "tests/verification/VerificationIO.hpp"

#include "simulation/Simulation.hpp"
#include "io/PointField.hpp"
#include "io/FieldWriter.hpp"
#include "postprocessing/BoundaryReconstructor.hpp"

#include "nlohmann/json.hpp"

#include <fstream>
#include <vector>

void VerificationIO::writeCSV(
    const Simulation& sim,
    const std::vector<double>& phi,
    const std::filesystem::path& file)
{
    PointField field =
        BoundaryReconstructor::reconstruct( sim.mesh(), sim.boundary(), sim.model(), phi);

    std::vector<double> residual(phi.size(), 0.0);

    FieldWriter::writeCSVDebug( field, sim.system().RHS(), residual, file.string());
}

void VerificationIO::writeSummary(
    const std::string& caseName,
    double l2,
    double linf,
    const std::filesystem::path& file)
{
    nlohmann::json j;

    j["case"]  = caseName;
    j["l2"]    = l2;
    j["linf"]  = linf;

    std::ofstream out(file);

    if (!out.is_open()) { throw std::runtime_error("Failed to open verification summary file: " + file.string()); }

    out << j.dump(4);
}
