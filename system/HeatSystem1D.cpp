#include "HeatSystem1D.hpp"
#include "discretization/Discretization1D.hpp"
#include <functional>
#include "Solver/SolverMethod.hpp"
#include <iostream>


HeatSystem1D::HeatSystem1D(int n, double L, double A, double k, bool output)
    : mesh_(n, L),
      sys_(n),
      A_(A),
      k_(k),
      output_(output)
{}
void HeatSystem1D::assemble()
{
    sys_.reset();

    discretize_1D(mesh_, sys_, k_, A_);

    for (const auto& bc : bcs_)
    {
        bc->apply(mesh_, sys_, k_, A_);
    }
}
void HeatSystem1D::setSource(
    std::function<double(double)> Su_func,
    std::function<double(double)> Sp_func)
{
    sys_.setSource(Su_func, Sp_func, mesh_.x);
}
void HeatSystem1D::addBC(std::unique_ptr<BoundaryCondition> bc)
{
    bcs_.push_back(std::move(bc));
}
std::vector<double> HeatSystem1D::solve(SolverMethod method, int iter, double tol, double omega)
{
    // std::cout << "Entering solve()\n";

    switch (method)
    {
        case SolverMethod::TDMA:
            return TDMA(sys_);
        case SolverMethod::GS:
            return GaussSeidel(sys_, iter, tol, output_);
        case SolverMethod::SOR:
            return SOR(sys_, iter, tol, omega, output_);
        default:
            throw std::runtime_error("Unknown solver method");
    }

}
