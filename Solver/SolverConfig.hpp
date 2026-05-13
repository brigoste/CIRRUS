#pragma

#include "Solver/SolverMethod.hpp"

struct SolverConfig
{
    SolverMethod method;
    int iter;
    double tol;
    double omega;
};
