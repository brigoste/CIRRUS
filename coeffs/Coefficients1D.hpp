#pragma once
#include <vector>

struct Coefficients1D {
    std::vector<double> aP, aE, aW, b;
    explicit Coefficients1D(int n);
    std::vector<double> Su;
    std::vector<double> Sp;
};
