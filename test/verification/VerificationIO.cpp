#include "test/verification/VerificationIO.hpp"

#include "simulation/Simulation.hpp"

#include "io/CSVWriter.hpp"
#include "io/VTKWriter.hpp"
#include "io/OutputData.hpp"
#include "io/PointField.hpp"
#include "io/FieldOutput.hpp"
#include "io/OutputBuilder.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"
#include "fields/ScalarField.hpp"

#include "nlohmann/json.hpp"

#include <fstream>
#include <stdexcept>

void VerificationIO::writeCSV(
    const Simulation& sim,
    const ScalarField& phi,
    const std::filesystem::path& file)
{
    auto output =
    OutputBuilder::build(
        sim,
        phi);

    CSVWriter::write(output, file);
}

void VerificationIO::writeVTK(
    const Simulation& sim,
    const ScalarField& phi,
    const std::filesystem::path& path)
{
    auto output =
    OutputBuilder::build(
        sim,
        phi);

    VTKWriter::write(output, path);
}

void VerificationIO::writeReport(
    const std::string& caseName,
    double l2,
    double linf,
    const std::filesystem::path& file)
{
    nlohmann::json j;

    j["verification"]["case"] = caseName;

    j["error"]["L2"] = l2;
    j["error"]["Linf"] = linf;

    std::ofstream out(file);

    if (!out.is_open())
    {
        throw std::runtime_error(
            "Failed to open verification summary JSON: "
            + file.string());
    }

    out << j.dump(4);
}
