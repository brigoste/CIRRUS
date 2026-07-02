#include "tests/verification/ManufacturedCases/1DLinear.hpp"
// #include "tests/verification/VerificationAutoRegister.hpp"
// #include "tests/verification/VerificationRegistry.hpp"
double OneDLinear::exact(double x, double) const
{
    return 300.0 + (100.0 / 1.5) * x;
}

double OneDLinear::laplacian(double,double) const
{
    return 0.0;
}
