#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct RefinementLevel
{
    std::size_t nx;
    std::size_t ny;

    double h;

    double l2;
    double linf;
};

struct RegressionResult
{
    double slope = 0.0;
    double intercept = 0.0;
    double rSquared = 0.0;
};

struct GCIResult
{
    double relativeError = 0.0;
    double gci = 0.0;
    double refinementRatio = 0.0;
    double observedOrder = 0.0;
};

struct GridConvergenceStudy
{
    std::string caseName;

    std::vector<RefinementLevel> levels;

    RegressionResult l2Regression;
    RegressionResult linfRegression;

    std::vector<double> l2Orders;
    std::vector<double> linfOrders;

    std::vector<GCIResult> l2GCI;
    std::vector<GCIResult> linfGCI;

    bool passed = true;
};