#include "test/verification/ManufacturedCases/1DLinear.hpp"

double OneDLinear::exact(Point p) const
{
    return 100.0 + 100.0 * p.x[0];
}

double OneDLinear::laplacian( Point ) const
{
    return 0.0;
}
