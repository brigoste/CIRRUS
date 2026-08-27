#pragma once

#include <string>
#include <stdexcept>

enum class GradientType
{
    GreenGauss,
    LeastSquares
};


inline GradientType gradientFromString(const std::string& name)
{
    if (name == "green_gauss")
        return GradientType::GreenGauss;

    if (name == "least_squares")
        return GradientType::LeastSquares;

    throw std::runtime_error( "Unknown gradient scheme: " + name );
}


inline std::string gradientToString(GradientType type)
{
    switch(type)
    {
        case GradientType::GreenGauss:
            return "green_gauss";

        case GradientType::LeastSquares:
            return "least_squares";
    }

    return "unknown";
}
