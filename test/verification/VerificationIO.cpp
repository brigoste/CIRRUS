#include "test/verification/VerificationIO.hpp"
#include "test/verification/VerificationSummary.hpp"

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
    const VerificationSummary& result,
    const std::filesystem::path& file)
{
    nlohmann::json j;

    j["verification"]["case"] = result.caseName;
    j["verification"]["solver"] = result.solver;

    j["verification"]["mesh"]["type"] = result.meshType;
    j["verification"]["mesh"]["size"] = result.meshSize;

    j["verification"]["gradient"] = result.gradient;
    j["verification"]["reconstruction"] = result.reconstruction;

    j["accuracy"]["L2"] = result.l2Error;
    j["accuracy"]["Linf"] = result.linfError;

    j["accuracy"]["L2Tolerance"] = result.l2AcceptanceTol;
    j["accuracy"]["LinfTolerance"] = result.linfAcceptanceTol;
    j["accuracy"]["passed"] = result.accuracyPassed;
    
    j["qoi"]["value"] = result.qoiValue;

    j["refinement"]["enabled"] = result.refinementEnabled;

    if (result.refinementEnabled)
    {
        j["refinement"]["L2Order"] = result.l2Order;
        j["refinement"]["LinfOrder"] = result.linfOrder;
        j["refinement"]["passed"] = result.refinementPassed;
        j["refinement"]["QoIOrder"] = result.qoiOrder;
    }

    j["passed"] = result.passed();

    std::ofstream out(file);

    if (!out.is_open())
    {
        throw std::runtime_error(
            "Failed to open verification summary JSON: "
            + file.string());
    }

    out << j.dump(4);
}
