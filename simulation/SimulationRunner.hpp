#pragma once

#include "config/SimulationConfig.hpp"
#include "config/PathContext.hpp"

enum class VisualizationMode
{
    None,
    Pyplot,
    Paraview
};

class SimulationRunner
{
public:

    static void run( const SimulationConfig& cfg, 
                     const PathContext& paths, 
                     VisualizationMode graphics_backend = VisualizationMode::Pyplot);
    static void validate( const SimulationConfig& cfg );
};
