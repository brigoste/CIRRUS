#pragma once

#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"

class SimulationRunner
{
public:

    static void run( const SimulationConfig& cfg, const PathContext& paths );
    static void validate( const SimulationConfig& cfg );
};
