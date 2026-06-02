#pragma once

#include <string>
#include <stdexcept>

namespace bc
{
    enum class Type
    {
        Dirichlet,
        Neumann,
        Convective,
        Mixed,
        None
    };

    inline Type from_string(const std::string& s)
    {
        if (s == "Dirichlet") return Type::Dirichlet;
        if (s == "Neumann")   return Type::Neumann;
        if (s == "Convective") return Type::Convective;
        if (s == "Mixed")     return Type::Mixed;
        if (s == "None")      return Type::None;

        throw std::runtime_error("Unknown BC type: " + s);
    }

    inline const char* to_string(Type t)
    {
        switch (t)
        {
            case Type::Dirichlet: return "Dirichlet";
            case Type::Neumann:   return "Neumann";
            case Type::Convective:return "Convective";
            case Type::Mixed:     return "Mixed";
            case Type::None:      return "None";
        }

        return "UNKNOWN";
    }
}
