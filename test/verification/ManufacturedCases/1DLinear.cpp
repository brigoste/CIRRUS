#include "test/verification/ManufacturedCases/1DLinear.hpp"

double OneDLinear::exact(double x, double) const
{
    return 100.0 + 100.0 * x;
}

double OneDLinear::laplacian(double,double) const
{
    return 0.0;
}
