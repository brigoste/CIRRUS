#include "physics/HeatEquationModel.hpp"

HeatEquationModel makeConstantSource(double su, double sp)
{
    HeatEquationModel m;
    m.Su = [=](const Point&) { return su; };
    m.Sp = [=](const Point&) { return sp; };
    return m;
}
