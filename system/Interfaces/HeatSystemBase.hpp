#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "bc/BoundaryCondition.hpp"
#include "system/LinearSystem.hpp"
#include "mesh/Mesh1D.hpp"
#include "Solver/SolverMethod.hpp"
#include "Solver/TDMA.hpp"
#include "Solver/GaussSeidel.hpp"

class HeatSystemBase {
public:
    virtual ~HeatSystemBase() = default;

    // -----------------------------------------
    // Core workflow
    // -----------------------------------------
    virtual void assemble() = 0;

    std::vector<double> solve(SolverMethod method,
                              int max_iter = 5000,
                              double tol = 1e-10,
                              double omega = 1.0);

    // -----------------------------------------
    // BC management
    // -----------------------------------------
    void addBC(std::unique_ptr<BoundaryCondition> bc) {
        bcs_.push_back(std::move(bc));
    }

    // -----------------------------------------
    // Source management
    // -----------------------------------------
    void setSource(std::function<double(double)> Su_func,
                   std::function<double(double)> Sp_func);

    void clearSource(const std::vector<double>& x);

protected:
    std::vector<std::unique_ptr<BoundaryCondition>> bcs_;

    // -----------------------------------------
    // Derived classes MUST give access to mesh & system
    // -----------------------------------------
    virtual Mesh1D& mesh() = 0;
    virtual LinearSystem& system() = 0;
};
