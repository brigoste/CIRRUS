#pragma once

#include "test/verification/VerificationSummary.hpp"
#include <filesystem>
#include <string>

class Simulation;
class ScalarField;

struct VerificationComparison
{
    bool passed = true;

    // -------------------------------------------------
    // Configuration consistency
    // -------------------------------------------------

    bool caseMatch = true;
    bool solverMatch = true;
    bool meshMatch = true;
    bool gradientMatch = true;
    bool reconstructionMatch = true;

    bool refinementEnabledMatch = false;

    // -------------------------------------------------
    // Accuracy
    // -------------------------------------------------

    bool l2Passed = true;
    bool linfPassed = true;
    bool qoiPassed = true;

    double l2Difference = 0.0;
    double linfDifference = 0.0;
    double qoiDifference = 0.0;

    // -------------------------------------------------
    // Observed order
    // -------------------------------------------------

    bool l2OrderPassed = true;
    bool linfOrderPassed = true;
    bool qoiOrderPassed = true;

    double l2OrderDifference = 0.0;
    double linfOrderDifference = 0.0;
    double qoiOrderDifference = 0.0;

    // -------------------------------------------------
    // Richardson extrapolation
    // -------------------------------------------------

    bool l2RichardsonPassed = true;
    bool linfRichardsonPassed = true;
    bool qoiRichardsonPassed = true;

    double l2RichardsonDifference = 0.0;
    double linfRichardsonDifference = 0.0;
    double qoiRichardsonDifference = 0.0;

    // -------------------------------------------------
    // Grid Convergence Index
    // -------------------------------------------------

    bool l2GCIPassed = true;
    bool linfGCIPassed = true;
    bool qoiGCIPassed = true;

    double l2GCIDifference = 0.0;
    double linfGCIDifference = 0.0;
    double qoiGCIDifference = 0.0;
};

class VerificationIO
{
public:

    static void writeCSV(
        const Simulation& sim,
        const ScalarField& phi,
        const std::filesystem::path& path);

    static void writeVTK(
        const Simulation& sim,
        const ScalarField& phi,
        const std::filesystem::path& path);

    static void writeReport(
        const VerificationSummary& result,
        const std::filesystem::path& file);

    VerificationSummary readBaseline(
        const std::filesystem::path& file);

    VerificationComparison compare(
        const VerificationSummary& current,
        const VerificationSummary& baseline
    );
};
