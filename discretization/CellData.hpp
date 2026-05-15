#pragma once

struct CellData
{
    double aP = 0.0;   // diagonal contributions (diffusion + BC + convection)
    double Su = 0.0;   // explicit source
    double Sp = 0.0;   // implicit sink (linearized)
};
