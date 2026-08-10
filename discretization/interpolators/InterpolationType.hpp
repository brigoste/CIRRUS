#pragma once

#include <string>
#include <stdexcept>

enum class InterpolationType
{
    CentralLinear,
    Upwind
};

inline InterpolationType interpolationFromString(const std::string& name)
{
    if (name == "centralLinear")
        return InterpolationType::CentralLinear;

    if (name == "upwind")
        return InterpolationType::Upwind;

    throw std::runtime_error(
        "Unknown interpolation scheme: " + name
    );
}

inline std::string interpolationToString(InterpolationType type)
{
    switch (type)
    {
        case InterpolationType::CentralLinear:
            return "centralLinear";

        case InterpolationType::Upwind:
            return "upwind";
    }

    return "unknown";
}