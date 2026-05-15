#pragma once

#include "system/Interfaces/HeatSystemBase.hpp"
#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "system/HeatCase1D.hpp"
#include "physics/HeatEquationModel.hpp"
#include "config/BoundaryConfig.hpp"
#include "Solver/SolverMethod.hpp"

class HeatSystem1D : public HeatSystemBase
{
public:
    HeatSystem1D(const MeshBase& mesh,
                 const HeatCase1D& problem);

    // lifecycle
    void assemble() override;

    std::vector<double> solve(
        SolverMethod method,
        int max_iter,
        double tol,
        double omega = 1.0
    ) override;

    // base interface
    const MeshBase& mesh() const override;
    const LinearSystem& system() const override;
    int size() const override;

    // void addBC(std::unique_ptr<BoundaryCondition>) override;

private:
    void applyDirichlet(const BoundaryConfig& bc,
                        const BoundaryContext& ctx);

    void applyNeumann(const BoundaryConfig& bc,
                      const BoundaryContext& ctx);

    void applyConvective(const BoundaryConfig& bc,
                         const BoundaryContext& ctx);

private:
    const MeshBase& mesh_;
    LinearSystem sys_;
    const HeatCase1D& problem_;
};
