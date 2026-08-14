#pragma once

#include <stdexcept>
#include <string>

enum class ReconstructionType
{
    Gradient,
    Central,
    Upwind,
    SecondOrderUpwind,
    QUICK,
    MUSCL,
    TVD
};

inline ReconstructionType reconstructionTypeFromString(const std::string& value)
{
    if (value == "Gradient")
        return ReconstructionType::Gradient;

    if (value == "Central")
        return ReconstructionType::Central;

    if (value == "Upwind")
        return ReconstructionType::Upwind;
    
    if (value == "secondOrderUpwind")
        return ReconstructionType::SecondOrderUpwind;
    
    if (value == "Quick")
        return ReconstructionType::QUICK;
    
    if (value == "MUSCL")
        return ReconstructionType::MUSCL;
    
    if (value == "TVD")
        return ReconstructionType::TVD;

    throw std::runtime_error(
        "Unknown reconstruction scheme: " + value
    );
}

inline std::string reconstructionToString(ReconstructionType type)
{
    switch (type)
    {
        case ReconstructionType::Gradient:
            return "Gradient";

        case ReconstructionType::Central:
            return "Central";

        case ReconstructionType::Upwind:
            return "Upwind";

        case ReconstructionType::SecondOrderUpwind:
            return "secondOrderUpwind";

        case ReconstructionType::QUICK:
            return "Quick";
        
        case ReconstructionType::MUSCL:
            return "MUSCL";
        
        case ReconstructionType::TVD:
            return "TVD";
    }

    throw std::runtime_error(
        "Unsupported reconstruction scheme."
    );
}
