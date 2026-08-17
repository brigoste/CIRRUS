#include "test/verification/VerificationAnalyzer.hpp"

#include <cmath>
#include <stdexcept>

namespace VerificationAnalyzer
{
    double observedOrder(
        double errorCoarse,
        double errorFine,
        double hCoarse,
        double hFine)
    {
        return std::log(errorCoarse / errorFine) / std::log(hCoarse / hFine);
    }

    std::vector<double> observedOrders(
        const std::vector<double>& errors,
        const std::vector<double>& h)
    {
        if (errors.size() != h.size()) { throw std::invalid_argument("errors and h must have the same size"); }
        if (errors.size() < 2) { throw std::invalid_argument( "At least two refinement levels are required"); }

        std::vector<double> orders;
        orders.reserve(errors.size() - 1);

        for (std::size_t i = 1; i < errors.size(); ++i)
        {
            orders.push_back(
                observedOrder(
                    errors[i - 1],
                    errors[i],
                    h[i - 1],
                    h[i]
                )
            );
        }

        return orders;
    }

    RegressionResult regression(
        const std::vector<double>& h,
        const std::vector<double>& errors)
    {
        if (h.size() != errors.size()) { throw std::invalid_argument( "h and errors must have the same size"); }
        if (h.size() < 2) { throw std::invalid_argument( "At least two points are required for regression"); }

        std::vector<double> x;
        std::vector<double> y;

        x.reserve(h.size());
        y.reserve(errors.size());

        for (std::size_t i = 0; i < h.size(); ++i)
        {
            x.push_back(std::log(h[i]));
            y.push_back(std::log(errors[i]));
        }

        const double n = static_cast<double>(x.size());

        double sumX = 0.0;
        double sumY = 0.0;
        double sumXY = 0.0;
        double sumX2 = 0.0;

        for (std::size_t i = 0; i < x.size(); ++i)
        {
            sumX += x[i];
            sumY += y[i];
            sumXY += x[i] * y[i];
            sumX2 += x[i] * x[i];
        }

        const double denominator = n * sumX2 - sumX * sumX;

        if (denominator == 0.0)
        {
            throw std::invalid_argument( "Cannot perform regression: h values are identical");
        }

        RegressionResult result;

        result.slope = (n * sumXY - sumX * sumY) / denominator;

        result.intercept = (sumY - result.slope * sumX) / n;

        double ssTotal = 0.0;
        double ssResidual = 0.0;

        const double meanY = sumY / n;

        for (std::size_t i = 0; i < y.size(); ++i)
        {
            const double predicted = result.slope * x[i] + result.intercept;

            ssTotal += std::pow(y[i] - meanY, 2.0);
            ssResidual += std::pow(y[i] - predicted, 2.0);
        }

        result.rSquared = 1.0 - ssResidual / ssTotal;

        return result;
    }

    double richardsonExtrapolation(
        double phiCoarse,
        double phiFine,
        double refinementRatio,
        double order)
    {
        return phiFine + (phiFine - phiCoarse) / (std::pow(refinementRatio, order) - 1.0);
    }

    GCIResult gridConvergenceIndex(
        double phiCoarse,
        double phiFine,
        double refinementRatio,
        double order,
        double safetyFactor)
    {
        GCIResult result;

        result.relativeError = std::abs( (phiFine - phiCoarse) / phiFine);

        result.refinementRatio = refinementRatio;
        result.observedOrder = order;

        result.gci = safetyFactor * result.relativeError / (std::pow(refinementRatio, order) - 1.0);

        return result;
    }

    GridConvergenceStudy analyzeRefinement(const std::vector<RefinementLevel>& levels)
    {
        if (levels.size() < 2)
        {
            throw std::invalid_argument(
                "At least two refinement levels are required"
            );
        }

        std::vector<double> h;
        std::vector<double> eq;
        std::vector<double> l2;
        std::vector<double> linf;

        for (const auto& level : levels)
        {
            h.push_back(level.h);
            eq.push_back(level.qoiError);
            l2.push_back(level.l2);
            linf.push_back(level.linf);
        }

        GridConvergenceStudy study;

        study.l2Regression = regression(h, l2);
        study.linfRegression = regression(h, linf);
        study.qoiRegression = regression(h,eq);
        study.l2Orders = observedOrders(l2, h);
        study.linfOrders = observedOrders(linf, h);
        study.qoiOrders = observedOrders(eq, h);

        const auto& coarse = levels[levels.size() - 2];
        const auto& fine   = levels[levels.size() - 1];

        const double refinementRatio = coarse.h / fine.h;
        const double qoiOrder = study.qoiRegression.slope;

        study.qoiRichardson = richardsonExtrapolation( coarse.qoiValue,
                                                       fine.qoiValue,
                                                       refinementRatio,
                                                       qoiOrder);
        

        return study;
    }
}
