#include "test/verification/VerificationIO.hpp"

#include "simulation/Simulation.hpp"

#include "io/CSVWriter.hpp"
#include "io/VTKWriter.hpp"
#include "io/OutputData.hpp"
#include "io/PointField.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"
#include "fields/ScalarField.hpp"

#include "nlohmann/json.hpp"

#include <fstream>
#include <vector>


void VerificationIO::writeCSV(
    const Simulation& sim,
    const ScalarField& phi,
    const std::filesystem::path& file)
{
    PointField reconstructed =
        BoundaryReconstructor::reconstruct(
            sim.mesh(),
            sim.boundary(),
            sim.model(),
            phi);

    std::vector<double> residual(
        phi.size(),
        0.0);


    OutputData output{
        sim.mesh(),
        phi,
        reconstructed,
        sim.system().RHS(),
        residual
    };


    CSVWriter::write(
        output,
        file);
}

void VerificationIO::writeVTK(
    const Simulation& sim,
    const ScalarField& phi,
    const std::filesystem::path& path)
{
    PointField reconstructed =
        BoundaryReconstructor::reconstruct(
            sim.mesh(),
            sim.boundary(),
            sim.model(),
            phi);

    std::vector<double> residual(
        phi.size(),
        0.0);

    OutputData data{
        sim.mesh(),
        phi,
        reconstructed,
        sim.system().RHS(),
        residual
    };

    VTKWriter::write(
        data,
        path);
}

void VerificationIO::writeSummary(
    const std::string& caseName,
    double l2,
    double linf,
    const std::filesystem::path& file)
{
    nlohmann::json j;

    j["case"] = caseName;
    j["l2"]   = l2;
    j["linf"] = linf;

    std::ofstream out(file);

    if (!out.is_open())
    {
        throw std::runtime_error(
            "Failed to open verification summary file: "
            + file.string());
    }

    out << j.dump(4);
}