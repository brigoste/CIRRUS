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

    double l2AcceptanceTol;
    double linfAcceptanceTol;

    // Individual verification checks
    bool accuracyPassed = false;

    bool refinementEnabled = false;
    bool refinementPassed = false;
    double l2Order = 0.0;
    double linfOrder = 0.0;

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