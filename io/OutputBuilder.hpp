#pragma once

struct OutputData;

class Simulation;
class ScalarField;

class OutputBuilder
{
public:

    static OutputData build(
        const Simulation& sim,
        const ScalarField& field);
};