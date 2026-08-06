#pragma once

#include <vector>

class MeshBase;
class ScalarField;

// computes L2/Linf norms

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
        const ScalarField& numerical,
        const std::vector<double>& exact
    );
};
