#pragma once

#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace solver
{
    // =========================================================
    // Solver type
    // =========================================================
    enum class Method
    {
        TDMA,
        GS,
        SOR,
        CG,
        BiCGSTAB
    };

    // =========================================================
    // String conversion
    // =========================================================
    inline Method from_string(const std::string& s)
    {
        if (s == "TDMA") return Method::TDMA;
        if (s == "GS")   return Method::GS;
        if (s == "SOR")  return Method::SOR;
        if (s == "CG")   return Method::CG;
        if (s == "BiCGSTAB") return Method::BiCGSTAB;

        throw std::runtime_error("Unknown solver method: " + s);
    }

    inline const char* to_string(Method m)
    {
        switch (m)
        {
            case Method::TDMA:      return "TDMA";
            case Method::GS:        return "GS";
            case Method::SOR:       return "SOR";
            case Method::CG:        return "CG";
            case Method::BiCGSTAB:  return "BiCGSTAB";
        }
        return "UNKNOWN";
    }

    // =========================================================
    // JSON serialization (nlohmann)
    // =========================================================
    inline void to_json(nlohmann::json& j, const Method& m)
    {
        j = to_string(m);
    }

    inline void from_json(const nlohmann::json& j, Method& m)
    {
        m = from_string(j.get<std::string>());
    }
}
