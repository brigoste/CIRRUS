#pragma once

#include <unordered_map>
#include <functional>

#include "bc/BCType.hpp"
#include "mesh/Face.hpp"
#include "linear_system/LinearSystem.hpp"

class BoundaryPatchSystem
{
public:
    struct Condition
    {
        double value = 0.0;
        double flux  = 0.0;
        double h     = 0.0;
        double Tinf  = 0.0;

        bc::Type type = bc::Type::None;
    };

    // -----------------------------
    // registration (by face index)
    // -----------------------------
    void set(std::size_t faceIndex, const Condition& bc);

    bool has(std::size_t faceIndex) const;

    const Condition* get(std::size_t faceIndex) const;

private:
    std::unordered_map<std::size_t, Condition> bcMap_;
};
