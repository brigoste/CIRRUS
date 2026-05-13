#pragma once
#include "Solver/SolverMethod.hpp"

struct SimulationConfig
{
    int n;
    double L;
    double A;
    double k;
    bool output;

    double Su;
    double Sp;

    double T_left;
    double T_right;

    // -------------------------
    // Solver settings
    // -------------------------
    SolverMethod method;
    int iter;
    double tol;
    double omega;

};
