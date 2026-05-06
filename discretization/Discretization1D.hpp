#pragma once
#include "mesh/Mesh1D.hpp"
#include "system/LinearSystem.hpp"
#include "system/HeatSystem1D.hpp"

void discretize_1D(
    const Mesh1D& mesh,
    LinearSystem& sys,
    double k,
    double A
);
