#pragma once

#include <vector>
#include <memory>
#include <functional>

// Core components
#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"
#include "bc/BoundaryCondition.hpp"

// Solver
#include "solver/TDMA.hpp"

class HeatSystem1D {
public:
    // Now system owns full physical state via Mesh1D
    HeatSystem1D(int n, double L, double A, double k);

    // Add boundary conditions (ownership transferred)
    void addBC(std::unique_ptr<BoundaryCondition> bc);

    // Build linear system (no physics arguments anymore)
    void assemble();

    // Set source terms using spatial functions Su(x) and Sp(x)
    void setSource(std::function<double(double)> Su_func,
                std::function<double(double)> Sp_func = nullptr);
    // Set uniform Su and Sp values
    void setConstantSource(double Su, double Sp = 0.0);

    // Clear all source terms (Su = 0, Sp = 0)
    void clearSource();

    // Solve system
    std::vector<double> solve();

    // Accessors (debug / post-processing)
    const Mesh1D& mesh() const;
    const Coefficients1D& coeffs() const;

private:
    Mesh1D mesh_;
    Coefficients1D coeffs_;

    std::vector<std::unique_ptr<BoundaryCondition>> bcs_;
};
