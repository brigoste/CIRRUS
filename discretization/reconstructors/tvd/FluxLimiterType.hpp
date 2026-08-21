#pragma once

#include <stdexcept>
#include <string>

enum class FluxLimiterType
{
    Minmod,
    Superbee,
    VanLeer,
    MC,
    VanAlbada
};


inline FluxLimiterType fluxLimiterTypeFromString(const std::string& value)
{
    if (value == "Minmod" || value == "minmod")
        return FluxLimiterType::Minmod;
    
    if (value == "Superbee" || value == "superbee")
        return FluxLimiterType::Superbee;

    if (value == "VanLeer" || value == "vanleer")
        return FluxLimiterType::VanLeer;
    
    if (value == "MC" || value == "mc")
        return FluxLimiterType::MC;
    
    if (value == "VanAlbada" || value == "vanalbada")
        return FluxLimiterType::VanAlbada;

    throw std::runtime_error(
        "Unknown Flux_limiter: " + value
    );
}

inline std::string fluxLimiterToString(FluxLimiterType type)
{
    switch (type)
    {
        case FluxLimiterType::Minmod:
            return "Minmod";
        case FluxLimiterType::Superbee:
            return "Superbee";
        case FluxLimiterType::VanLeer:
            return "VanLeer";
        case FluxLimiterType::MC:
            return "MC";
        case FluxLimiterType::VanAlbada:
            return "VanAlbada";
    }

    throw std::runtime_error(
        "Unsupported Flux_limiter."
    );
}
