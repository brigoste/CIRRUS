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
    if (name == "centralLinear")
        return ConvectionType::CentralLinear;

    if (name == "upwind")
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
            return "centralLinear";

        case ConvectionType::Upwind:
            return "upwind";
    }

    return "unknown";
}