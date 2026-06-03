#pragma once

#include <vector>

class ErrorNorms
{
public:

    static double L1(
        const std::vector<double>& numerical,
        const std::vector<double>& exact);

    static double L2(
        const std::vector<double>& numerical,
        const std::vector<double>& exact);

    static double Linf(
        const std::vector<double>& numerical,
        const std::vector<double>& exact);
};
