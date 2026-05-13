#pragma once
#include <string>

enum class SolverMethod
{
    TDMA,
    GS,
    SOR,
    CG
};

inline std::string to_string(SolverMethod m)
{
    switch (m)
    {
        case SolverMethod::TDMA: return "TDMA";
        case SolverMethod::GS:   return "GS";
        case SolverMethod::SOR:  return "SOR";
        case SolverMethod::CG: return "CG";
        default: return "UNKNOWN";
    }
}
