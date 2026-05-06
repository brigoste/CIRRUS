#pragma once

#include <vector>
#include <memory>
#include <functional>  
#include "mesh/Mesh1D.hpp"
#include "system/LinearSystem.hpp"
#include "bc/BoundaryCondition.hpp"
#include "Solver/SolverMethod.hpp"
#include "Solver/TDMA.hpp"
#include "Solver/SOR.hpp"
#include "Solver/GaussSeidel.hpp"


class HeatSystem1D
{
    public:
        HeatSystem1D(int n, double L, double A, double k);

        void addBC(std::unique_ptr<BoundaryCondition> bc);

        double getCoordinate(int i) const;

        void assemble();

        std::vector<double> solve(
            SolverMethod method,
            int iter=1000,
            double tol=1e-5,
            double omega = 1.2
        );

        void setSource(
            std::function<double(double)> Su_func,
            std::function<double(double)> Sp_func
        );

        const Mesh1D& mesh() const        { return mesh_; }
        LinearSystem& system()            { return sys_; }
        const LinearSystem& system() const { return sys_; }

    private:
        Mesh1D mesh_;
        LinearSystem sys_;
        std::vector<std::unique_ptr<BoundaryCondition>> bcs_;

        double A_;
        double k_;
};
