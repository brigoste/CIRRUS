#pragma once

#include <string>
#include <stdexcept>

namespace bc
{
    enum class Type
    {
        Dirichlet,
        Neumann,
        Robin,
        Radiative,
        Mixed,
        None
    };

    inline Type from_string(const std::string& s)
    {
        if (s == "Dirichlet")  return Type::Dirichlet;
        if (s == "Neumann")    return Type::Neumann;
        if (s == "Robin")      return Type::Robin;
        if (s == "Radiative")  return Type::Radiative;
        if (s == "Mixed")      return Type::Mixed;
        if (s == "None")       return Type::None;

        throw std::runtime_error("Unknown BC type: " + s);
    }

    inline const char* to_string(Type t)
    {
        switch (t)
        {
            case Type::Dirichlet:  return "Dirichlet";
            case Type::Neumann:    return "Neumann";
            case Type::Robin:      return "Robin";
            case Type::Radiative:  return "Radiative";
            case Type::Mixed:      return "Mixed";
            case Type::None:       return "None";
        }

        return "UNKNOWN";
    }
}
