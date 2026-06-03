#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>

#include "Solver/SolverMethod.hpp"
#include "mesh/Point.hpp"
#include "mesh/BoundaryPatchSystem.hpp"

// -----------------------------
// Forward-safe definitions
// -----------------------------

struct BoundaryConfig
{
    std::size_t faceIndex;
    BoundaryPatchSystem::Condition condition;
};

struct MeshConfig
{
    std::string type = "uniform";
    int n = 0;
    double L = 0.0;
};

struct PhysicsConfig
{
    std::string type;   // "heat", "navier-stokes", etc.

    double k = 0.0;
};

struct SimulationConfig
{
    MeshConfig mesh;
    PhysicsConfig physics;
    std::vector<BoundaryConfig> boundary;  // <-- ALSO FIXED (was single object)

    struct SolverConfig
    {
        solver::Method method;
        int max_iter;
        double tol;
        double omega;
    } solver;
};

// factories
SimulationConfig defaultConfig();
SimulationConfig loadConfig(const std::filesystem::path& path);
