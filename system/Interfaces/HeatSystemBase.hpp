#pragma once

#include <vector>
#include <memory>

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "bc/BoundaryCondition.hpp"
#include "Solver/SolverMethod.hpp"

class HeatSystemBase
{
public:
    virtual ~HeatSystemBase() = default;

    // -----------------------------
    // lifecycle
    // -----------------------------
    virtual void assemble() = 0;

    virtual std::vector<double> solve(
        SolverMethod method,
        int max_iter,
        double tol,
        double omega = 1.0) = 0;

    // -----------------------------
    // system access
    // -----------------------------
    virtual const MeshBase& mesh() const = 0;
    virtual const LinearSystem& system() const = 0;

    virtual int size() const = 0;

    // -----------------------------
    // BC interface
    // -----------------------------
    // virtual void addBC(std::unique_ptr<BoundaryCondition>) = 0;
};
