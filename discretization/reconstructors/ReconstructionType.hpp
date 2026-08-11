#pragma once

#include <stdexcept>
#include <string>

enum class ReconstructionType
{
    Gradient,
    Central
};

inline ReconstructionType reconstructionTypeFromString(const std::string& value)
{
    if (value == "Gradient")
        return ReconstructionType::Gradient;

    if (value == "Central")
        return ReconstructionType::Central;

    throw std::runtime_error(
        "Unknown reconstruction scheme: " + value
    );
}

inline std::string toString(ReconstructionType type)
{
    switch (type)
    {
        case ReconstructionType::Gradient:
            return "Gradient";

        case ReconstructionType::Central:
            return "Central";
    }

    throw std::runtime_error(
        "Unsupported reconstruction scheme."
    );
}
