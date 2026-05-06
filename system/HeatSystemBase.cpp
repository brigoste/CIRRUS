#include "HeatSystemBase.hpp"
#include "Solver/TDMA.hpp"
#include "Solver/GaussSeidel.hpp"
#include "Solver/SOR.hpp"

std::vector<double> HeatSystemBase::solve(
        SolverMethod method, int max_iter, double tol, double omega)
{
    LinearSystem& sys = system();

    switch (method) {
    case SolverMethod::TDMA:
        return TDMA(sys);
    case SolverMethod::GS:
        return GaussSeidel(sys, max_iter, tol);
    case SolverMethod::SOR:
        return SOR(sys, max_iter, tol, omega);
    default:
        throw std::runtime_error("Unknown solver method");
    }
}

void HeatSystemBase::setSource(
        std::function<double(double)> Su_func,
        std::function<double(double)> Sp_func)
{
    Mesh1D& m = mesh();
    LinearSystem& s = system();
    s.setSource(Su_func, Sp_func, m.x);
}

void HeatSystemBase::clearSource(const std::vector<double>& x)
{
    system().clearSource(x);
}
