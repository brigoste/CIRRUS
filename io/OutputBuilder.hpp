#pragma once

#include <vector>

struct OutputData;

class Simulation;
class ScalarField;

class OutputBuilder
{
public:

    static OutputData build(
        const Simulation& sim,
        const ScalarField& field,
        const std::vector<double>& residual = {});
};