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

namespace
{
    bool approximatelyEqual(
        double current,
        double baseline,
        double relativeTolerance,
        double absoluteTolerance)
    {
        const double difference = std::abs(current - baseline);

        if (difference <= absoluteTolerance) { return true; }

        const double scale = std::max(std::abs(baseline), std::abs(current));

        return difference <= relativeTolerance * scale;
    }
}

void VerificationIO::writeCSV(
    const Simulation& sim,
    const ScalarField& phi,
    const std::filesystem::path& file)
{
    auto output = OutputBuilder::build( sim, phi);

    CSVWriter::write(output, file);
}

void VerificationIO::writeVTK(
    const Simulation& sim,
    const ScalarField& phi,
    const std::filesystem::path& path)
{
    auto output = OutputBuilder::build( sim, phi);

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

    // -------------------------------------------------
    // Accuracy
    // -------------------------------------------------

    j["accuracy"]["L2"] = result.l2Error;
    j["accuracy"]["Linf"] = result.linfError;

    j["accuracy"]["L2Tolerance"] = result.l2AcceptanceTol;
    j["accuracy"]["LinfTolerance"] = result.linfAcceptanceTol;

    j["accuracy"]["passed"] = result.accuracyPassed;

    // -------------------------------------------------
    // Quantity of Interest
    // -------------------------------------------------

    j["qoi"]["value"] = result.qoiValue;

    // -------------------------------------------------
    // Refinement
    // -------------------------------------------------

    j["refinement"]["enabled"] = result.refinementEnabled;

    if (result.refinementEnabled)
    {
        j["refinement"]["refinementRatio"] = result.refinementRatio;
        j["refinement"]["safetyFactor"] = result.safetyFactor;

        // Observed order
        j["refinement"]["L2Order"] = result.l2Order;
        j["refinement"]["LinfOrder"] = result.linfOrder;
        j["refinement"]["QoIOrder"] = result.qoiOrder;

        // Richardson extrapolation
        j["refinement"]["Richardson"]["L2"] = result.l2Richardson;
        j["refinement"]["Richardson"]["Linf"] = result.linfRichardson;
        j["refinement"]["Richardson"]["QoI"] = result.qoiRichardson;

        // Grid Convergence Index
        j["refinement"]["GCI"]["Relative"]["L2"] = result.l2RelativeGCI;
        j["refinement"]["GCI"]["Relative"]["Linf"] = result.linfRelativeGCI;
        j["refinement"]["GCI"]["Relative"]["QoI"] = result.qoiRelativeGCI;

        j["refinement"]["GCI"]["Absolute"]["L2"] = result.l2AbsoluteGCI;
        j["refinement"]["GCI"]["Absolute"]["Linf"] = result.linfAbsoluteGCI;
        j["refinement"]["GCI"]["Absolute"]["QoI"] = result.qoiAbsoluteGCI;

        // Refinement verification status
        j["refinement"]["passed"] = result.refinementPassed;
    }

    // -------------------------------------------------
    // Overall result
    // -------------------------------------------------

    j["passed"] = result.passed();

    std::ofstream out(file);

    if (!out.is_open()) { throw std::runtime_error( "Failed to open verification summary JSON: " + file.string()); }

    out << j.dump(4);
}

VerificationSummary VerificationIO::readBaseline(
    const std::filesystem::path& file)
{
    std::ifstream in(file);

    if (!in.is_open()) { throw std::runtime_error( "Failed to open verification baseline JSON: " + file.string()); }

    nlohmann::json j;
    in >> j;

    VerificationSummary result;

    result.caseName = j["verification"]["case"].get<std::string>();
    result.solver = j["verification"]["solver"].get<std::string>();

    result.meshType = j["verification"]["mesh"]["type"].get<std::string>();
    result.meshSize = j["verification"]["mesh"]["size"].get<std::string>();

    result.gradient = j["verification"]["gradient"].get<std::string>();
    result.reconstruction = j["verification"]["reconstruction"].get<std::string>();
    
    result.l2Error = j["accuracy"]["L2"].get<double>();
    result.linfError = j["accuracy"]["Linf"].get<double>();
    result.l2AcceptanceTol = j["accuracy"]["L2Tolerance"].get<double>();
    result.linfAcceptanceTol = j["accuracy"]["LinfTolerance"].get<double>();

    result.accuracyPassed = j["accuracy"]["passed"].get<bool>();

    if (j.contains("qoi")) { result.qoiValue = j["qoi"]["value"].get<double>(); }

    result.refinementEnabled = j["refinement"]["enabled"].get<bool>();

    if (result.refinementEnabled)
    {
        result.l2Order = j["refinement"]["L2Order"].get<double>();
        result.linfOrder = j["refinement"]["LinfOrder"].get<double>();
        result.qoiOrder = j["refinement"]["QoIOrder"].get<double>();

        result.refinementRatio = j["refinement"]["refinementRatio"].get<double>();
        result.safetyFactor = j["refinement"]["safetyFactor"].get<double>();

        result.l2Richardson = j["refinement"]["Richardson"]["L2"].get<double>();
        result.linfRichardson = j["refinement"]["Richardson"]["Linf"].get<double>();
        result.qoiRichardson = j["refinement"]["Richardson"]["QoI"].get<double>();

        result.l2AbsoluteGCI = j["refinement"]["GCI"]["Absolute"]["L2"].get<double>();
        result.linfAbsoluteGCI = j["refinement"]["GCI"]["Absolute"]["Linf"].get<double>();
        result.qoiAbsoluteGCI = j["refinement"]["GCI"]["Absolute"]["QoI"].get<double>();

        result.l2RelativeGCI = j["refinement"]["GCI"]["Relative"]["L2"].get<double>();
        result.linfRelativeGCI = j["refinement"]["GCI"]["Relative"]["Linf"].get<double>();
        result.qoiRelativeGCI = j["refinement"]["GCI"]["Relative"]["QoI"].get<double>();

        result.refinementPassed = j["refinement"]["passed"].get<bool>();
    }

    return result;
}

VerificationComparison VerificationIO::compare(
    const VerificationSummary& current,
    const VerificationSummary& baseline)
{
    VerificationComparison result;

    // -------------------------------------------------
    // Configuration consistency
    // -------------------------------------------------

    result.caseMatch = (current.caseName == baseline.caseName);
    result.solverMatch = (current.solver == baseline.solver);
    result.meshMatch = (current.meshType == baseline.meshType) && (current.meshSize == baseline.meshSize);
    result.gradientMatch = (current.gradient == baseline.gradient);
    result.reconstructionMatch = (current.reconstruction == baseline.reconstruction);

    // -------------------------------------------------
    // Accuracy results
    // -------------------------------------------------

    constexpr double relativeTolerance = 1e-10;
    constexpr double absoluteTolerance = 1e-14;

    result.l2Difference = current.l2Error - baseline.l2Error;

    result.linfDifference = current.linfError - baseline.linfError;

    result.l2Passed = approximatelyEqual( current.l2Error,
                                          baseline.l2Error,
                                          relativeTolerance,
                                          absoluteTolerance );

    result.linfPassed = approximatelyEqual( current.linfError,
                                            baseline.linfError,
                                            relativeTolerance,
                                            absoluteTolerance );

    // -------------------------------------------------
    // QoI
    // -------------------------------------------------

    result.qoiDifference = current.qoiValue - baseline.qoiValue;

    result.qoiPassed = approximatelyEqual( current.qoiValue,
                                           baseline.qoiValue,
                                           relativeTolerance,
                                           absoluteTolerance );

    result.refinementEnabledMatch = (current.refinementEnabled == baseline.refinementEnabled);

    result.passed = result.caseMatch &&
                    result.solverMatch &&
                    result.meshMatch &&
                    result.gradientMatch &&
                    result.reconstructionMatch &&
                    result.l2Passed &&
                    result.linfPassed &&
                    result.qoiPassed &&
                    result.refinementEnabledMatch;                                   

    if (current.refinementEnabled && baseline.refinementEnabled)
    {
        result.l2OrderDifference = current.l2Order - baseline.l2Order;
        result.linfOrderDifference = current.linfOrder - baseline.linfOrder;
        result.qoiOrderDifference = current.qoiOrder - baseline.qoiOrder;

        result.l2OrderPassed = approximatelyEqual( current.l2Order,
                                                   baseline.l2Order,
                                                   1e-3,
                                                   1e-6);

        result.linfOrderPassed = approximatelyEqual( current.linfOrder,
                                                     baseline.linfOrder,
                                                     1e-3,
                                                     1e-6);

        result.qoiOrderPassed = approximatelyEqual( current.qoiOrder,
                                                    baseline.qoiOrder,
                                                    1e-3,
                                                    1e-6);

        // -------------------------------------------------
        // Richardson extrapolation
        // -------------------------------------------------

        result.l2RichardsonDifference = (current.l2Richardson - baseline.l2Richardson);

        result.linfRichardsonDifference = (current.linfRichardson - baseline.linfRichardson);

        result.qoiRichardsonDifference = (current.qoiRichardson - baseline.qoiRichardson);

        result.l2RichardsonPassed = approximatelyEqual( current.l2Richardson,
                                                        baseline.l2Richardson,
                                                        1e-3,
                                                        1e-8);

        result.linfRichardsonPassed = approximatelyEqual( current.linfRichardson,
                                                          baseline.linfRichardson,
                                                          1e-3,
                                                          1e-8);

        result.qoiRichardsonPassed = approximatelyEqual( current.qoiRichardson,
                                                         baseline.qoiRichardson,
                                                         1e-3,
                                                         1e-8);

        // -------------------------------------------------
        // Grid Convergence Index
        // -------------------------------------------------

        result.l2GCIDifference = current.l2RelativeGCI - baseline.l2RelativeGCI;

        result.linfGCIDifference = current.linfRelativeGCI - baseline.linfRelativeGCI;

        result.qoiGCIDifference = current.qoiRelativeGCI - baseline.qoiRelativeGCI;

        result.l2GCIPassed = approximatelyEqual( current.l2RelativeGCI,
                                                 baseline.l2RelativeGCI,
                                                 1e-2,
                                                 1e-8);

        result.linfGCIPassed = approximatelyEqual( current.linfRelativeGCI,
                                                   baseline.linfRelativeGCI,
                                                   1e-2,
                                                   1e-8);

        result.qoiGCIPassed = approximatelyEqual( current.qoiRelativeGCI,
                                                  baseline.qoiRelativeGCI,
                                                  1e-2,
                                                  1e-8);

        result.passed = result.passed &&
                        result.l2OrderPassed &&
                        result.linfOrderPassed &&
                        result.qoiOrderPassed &&
                        result.l2RichardsonPassed &&
                        result.linfRichardsonPassed &&
                        result.qoiRichardsonPassed &&
                        result.l2GCIPassed &&
                        result.linfGCIPassed &&
                        result.qoiGCIPassed;
    }    

    return result;
}
