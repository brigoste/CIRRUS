#pragma once

#include <vector>

#include "mesh/MeshBase.hpp"

class DerivedFields
{
public:

    static std::vector<double> gradient1D(
        const MeshBase& mesh,
        const std::vector<double>& phi);

    static std::vector<double> heatFlux1D(
        const MeshBase& mesh,
        const std::vector<double>& phi,
        double conductivity);
};
