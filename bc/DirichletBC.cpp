#include "DirichletBC.hpp"
#include "system/LinearSystem.hpp"
#include <iostream> //debugging

DirichletBC::DirichletBC(int face, double value)
    : face_(face), value_(value) {}

void DirichletBC::apply([[maybe_unused]] const Mesh1D& m, LinearSystem& sys, [[maybe_unused]] double k, [[maybe_unused]] double A) const
{
    int i = face_;

    sys.aP[i] = 1.0;
    sys.aW[i] = 0.0;
    sys.aE[i] = 0.0;
    sys.b[i]  = value_;
}
