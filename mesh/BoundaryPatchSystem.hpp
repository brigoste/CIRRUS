#pragma once

#include <unordered_map>
#include "bc/BCType.hpp"
// #include "config/SimulationConfig.hpp"

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

    // assign BC to a group (LEFT, RIGHT, TOP, etc.)
    void setGroup(std::size_t group, const Condition& bc);
    const Condition* getGroup(std::size_t group) const;

private:
    std::unordered_map<std::size_t, Condition> groupBC_;
};
