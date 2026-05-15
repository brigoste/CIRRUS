#pragma once

struct CellResidual
{
    double aP = 0.0;   // implicit diagonal
    double Su = 0.0;   // explicit RHS
    double Sp = 0.0;   // linearized sink
};
