#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "Solver/SolverMethod.hpp"
// #include "bc/BoundaryConditionDescriptor.hpp"
#include "config/BoundaryConfig.hpp"

// ============================================================
// MESH CONFIGURATION
// ============================================================
struct MeshConfig
{
    std::string type = "uniform";
    int n = 0;
    double L = 0.0;
};

// ============================================================
// PHYSICS PARAMETERS
// ============================================================
struct PhysicsConfig
{
    double k = 0.0;
    double A = 1.0;
};

// ============================================================
// SOURCE TERMS
// ============================================================
struct SourceConfig
{
    double Su = 0.0;
    double Sp = 0.0;
};

// ============================================================
// SOLVER SETTINGS
// ============================================================
struct SolverConfig
{
    SolverMethod type = SolverMethod::TDMA;
    double tol = 1e-8;
    int max_iter = 5000;
    double omega = 1.0;
};

// ============================================================
// GLOBAL SIMULATION CONFIG
// ============================================================
struct SimulationConfig
{
    MeshConfig mesh;
    PhysicsConfig physics;
    SourceConfig source;
    SolverConfig solver;

    std::vector<BoundaryConfig> bcs;
};

// ============================================================
// FREE FUNCTIONS (NOT MEMBERS)
// ============================================================
SimulationConfig defaultConfig();
SimulationConfig loadConfig(const std::filesystem::path& path);
