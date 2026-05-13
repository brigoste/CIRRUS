#include "physics/HeatEquationModel.hpp"

// intentionally minimal
// physics layer is just data + optional lambdas
struct HeatEquationModel
{
    double k = 0.0;
    double Su = 0.0;
    double Sp = 0.0;
};
