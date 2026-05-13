#pragma once

#include "bc/BoundaryFace.hpp"

enum class BCType
{
    Dirichlet,
    Neumann,
    Convective
};

struct BoundaryConditionDescriptor
{
    BCType type;

    BoundaryFace face;   // ← key change

    double value = 0.0;  // Dirichlet
    double flux  = 0.0;  // Neumann
    double h     = 0.0;  // Convective
    double Tinf  = 0.0;

    static BoundaryConditionDescriptor Dirichlet(BoundaryFace f, double T)
    {
        return {BCType::Dirichlet, f, T, 0.0, 0.0, 0.0};
    }

    static BoundaryConditionDescriptor Neumann(BoundaryFace f, double q)
    {
        BoundaryConditionDescriptor bc;
        bc.type = BCType::Neumann;
        bc.face = f;
        bc.flux = q;
        return bc;
    }

    static BoundaryConditionDescriptor Convective(BoundaryFace f, double h, double Tinf)
    {
        BoundaryConditionDescriptor bc;
        bc.type = BCType::Convective;
        bc.face = f;
        bc.h = h;
        bc.Tinf = Tinf;
        return bc;
    }
};
