#include "coeffs/Coefficients1D.hpp"

Coefficients1D::Coefficients1D(int n)
    : aP(n), aE(n), aW(n), b(n)
    
{
    Su.assign(n, 0.0);
    Sp.assign(n, 0.0);
}
