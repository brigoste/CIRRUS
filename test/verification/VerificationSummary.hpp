#pragma once
#include <string>

// Verficiation Case Summary
struct VerificationSummary
{
    std::string caseName;

    // Configuration
    std::string solver;
    std::string meshType;
    std::string meshSize;
    std::string gradient;
    std::string reconstruction;

    // Accuracy
    double l2Error = 0.0;
    double linfError = 0.0;
    double qoiValue = 0.0;

    double l2AcceptanceTol = 0.0;
    double linfAcceptanceTol = 0.0;

    bool accuracyPassed = false;

    // Mesh refinement
    bool refinementEnabled = false;
    bool refinementPassed = false;

    double refinementRatio = 0.0;
    double safetyFactor = 1.25;

    double l2Order = 0.0;
    double linfOrder = 0.0;
    double qoiOrder = 0.0;

    // Richardson extrapolation
    double l2Richardson = 0.0;
    double linfRichardson = 0.0;
    double qoiRichardson = 0.0;

    // Grid Convergence Index
    double l2AbsoluteGCI = 0.0;
    double linfAbsoluteGCI = 0.0;
    double qoiAbsoluteGCI = 0.0;

    double l2RelativeGCI = 0.0;
    double linfRelativeGCI = 0.0;
    double qoiRelativeGCI = 0.0;

    bool passed() const
    {
        return accuracyPassed && (!refinementEnabled || refinementPassed);
    }
};
