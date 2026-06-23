#pragma once

#include <vector>

// computes L2/Linf norms

class MeshBase;

struct ErrorNormResults
{
    double l2_energy = 0.0;
    double l2_rms    = 0.0;
    double linf      = 0.0;
};

class ErrorNorms
{
public:

    static ErrorNormResults compute(
        const MeshBase& mesh,
        const std::vector<double>& numerical,
        const std::vector<double>& exact);
};
