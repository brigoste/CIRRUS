#pragma once

#include <vector>

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "solver/SolverMethod.hpp"
#include "physics/HeatEquationModel.hpp"

class HeatSystemBase
{
public:
    virtual ~HeatSystemBase() = default;

    // -----------------------------
    // lifecycle
    // -----------------------------
    virtual void assemble(const HeatEquationModel& model) = 0;

    virtual std::vector<double> solve(
        solver::Method method,
        int max_iter,
        double tol,
        double omega = 1.0) = 0;

    // -----------------------------
    // system access
    // -----------------------------
    virtual const MeshBase& mesh() const = 0;
    virtual const LinearSystem& system() const = 0;

    virtual int size() const = 0;
};
