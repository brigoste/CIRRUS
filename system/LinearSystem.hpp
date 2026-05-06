#pragma once

#include <vector>
#include <functional>

class LinearSystem {
public:
    explicit LinearSystem(int n);

    void reset();

    // -----------------------------
    // Source term handling
    // -----------------------------
    void setSource(std::function<double(double)> Su_func,
                   std::function<double(double)> Sp_func,
                   const std::vector<double>& x);
    void clearSource(const std::vector<double>& x);

    // -----------------------------
    // Data
    // -----------------------------
    std::vector<double> aP, aE, aW;
    std::vector<double> b;

    std::vector<double> x;
    std::vector<double> x_old;

    std::vector<double> Su;
    std::vector<double> Sp;
};
