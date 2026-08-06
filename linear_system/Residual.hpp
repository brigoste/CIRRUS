#pragma once

#include <vector>

class LinearSystem;
class ScalarField;

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const std::vector<double>& x);

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const ScalarField& field);
