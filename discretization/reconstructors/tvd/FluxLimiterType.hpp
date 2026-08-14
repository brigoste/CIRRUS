#pragma once

#include <stdexcept>
#include <string>

enum class FluxLimiterType
{
    Minmod
};


inline FluxLimiterType fluxLimiterTypeFromString(const std::string& value)
{
    if (value == "Minmod")
        return FluxLimiterType::Minmod;

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
    }

    throw std::runtime_error(
        "Unsupported Flux_limiter."
    );
}
