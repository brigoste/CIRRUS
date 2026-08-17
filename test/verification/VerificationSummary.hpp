#pragma once
#include <string>

// Verficiation Case Summary
struct VerificationSummary
{
    std::string caseName;
    std::string solver;
    std::string meshType;
    std::string meshSize;
    std::string gradient;
    std::string reconstruction;

    double l2Error;
    double linfError;
    double qoiValue = 0.0;

    double l2AcceptanceTol;
    double linfAcceptanceTol;

    // Individual verification checks
    bool accuracyPassed = false;

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

    // Overall case status
    bool passed() const
    {
        if (refinementEnabled)
        {
            return accuracyPassed && refinementPassed;
        }

        return accuracyPassed;
    }
};
