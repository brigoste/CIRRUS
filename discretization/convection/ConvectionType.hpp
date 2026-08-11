#pragma once

#include <string>
#include <stdexcept>

enum class ConvectionType
{
    CentralLinear,
    Upwind
};

inline ConvectionType convectionFromString(const std::string& name)
{
    if (name == "CentralLinear")
        return ConvectionType::CentralLinear;

    if (name == "Upwind")
        return ConvectionType::Upwind;

    throw std::runtime_error(
        "Unknown convection scheme: " + name
    );
}

inline std::string convectionToString(ConvectionType type)
{
    switch (type)
    {
        case ConvectionType::CentralLinear:
            return "CentralLinear";

        case ConvectionType::Upwind:
            return "Upwind";
    }

    return "unknown";
}
