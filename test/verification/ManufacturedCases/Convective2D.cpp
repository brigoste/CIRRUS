#include "test/verification/ManufacturedCases/Convective2D.hpp"
#include "utils/MathConstants.hpp"

double Convective2D::exact(double x, double y) const
{
    return T0_
         + a_*x
         + c_*x*x
         + d_*std::cos(math::PI*y/Ly_);
}

double Convective2D::laplacian(double /*x*/, double y) const
{
    return 2.0*c_
         - d_*std::pow(math::PI/Ly_,2)
           *std::cos(math::PI*y/Ly_);
}

double Convective2D::source(double x, double y) const
{
    return -k_ * laplacian(x,y);
}

double Convective2D::boundaryFlux(const Face& face) const
{
    const double x = face.center.x[0];
    const double y = face.center.x[1];

    const double dTdx = a_ + 2.0*c_*x;

    const double dTdy = -d_*(math::PI/Ly_) * std::sin(math::PI*y/Ly_);

    const double dTdn = dTdx*face.normal.x[0] + dTdy*face.normal.x[1];

    return -k_ * dTdn;
}

ConvectiveData Convective2D::boundaryConvective(const Face& face) const
{
    ConvectiveData convective{};

    constexpr double h = 10.0;

    const double x = face.center.x[0];
    const double y = face.center.x[1];

    const double T = exact(x,y);

    const double dTdx = a_ + 2.0*c_*x;

    const double dTdy = -d_*(math::PI/Ly_) *std::sin(math::PI*y/Ly_);

    const double dTdn = dTdx*face.normal.x[0] + dTdy*face.normal.x[1];

    convective.h = h;
    convective.T_inf = T + (k_*dTdn)/h;

    return convective;
}
