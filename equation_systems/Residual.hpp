#pragma once

#include <vector>

class LinearEquationSystem;
class ScalarField;

std::vector<double> computeResidual(
    const LinearEquationSystem& sys,
    const std::vector<double>& x);

std::vector<double> computeResidual(
    const LinearEquationSystem& sys,
    const ScalarField& field);
