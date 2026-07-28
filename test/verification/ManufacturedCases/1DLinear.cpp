#include "tests/verification/ManufacturedCases/1DLinear.hpp"

double OneDLinear::exact(double x, double) const
{
    return k_ * (3 + (1 / 1.5) * x);
}

double OneDLinear::laplacian(double,double) const
{
    return 0.0;
}
