#pragma once

#include "test/verification/VerificationTools.hpp"
#include "mesh/primitives/Point.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

namespace VerificationAnalyzer
{

    enum class QuantityType
    {
        ScalarValue,
        VectorMagnitude
    };

    struct QuantityOfInterest
    {
        QuantityType type;
        Point evalPoint;

        const ScalarField* scalarField = nullptr;
        const VectorField* vectorField = nullptr;
    };

    double observedOrder(
        double errorCoarse,
        double errorFine,
        double hCoarse,
        double hFine);

    std::vector<double> observedOrders(
        const std::vector<double>& errors,
        const std::vector<double>& h);

    RegressionResult regression(
        const std::vector<double>& h,
        const std::vector<double>& errors);

    double richardsonExtrapolation(
        double phiCoarse,
        double phiFine,
        double refinementRatio,
        double order);

    GCIResult gridConvergenceIndex(
        double phiCoarse,
        double phiFine,
        double refinementRatio,
        double order,
        double safetyFactor = 1.25);

    GridConvergenceStudy analyzeRefinement(
        const std::vector<RefinementLevel>& levels);
}