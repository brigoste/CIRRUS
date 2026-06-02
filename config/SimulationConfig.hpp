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
    double k = 0.0;
};

struct SourceConfig
{
    std::function<double(const Point&)> Su;
    std::function<double(const Point&)> Sp;
};

struct SimulationConfig
{
    MeshConfig mesh;
    PhysicsConfig physics;
    SourceConfig source;              // <-- REQUIRED FIX
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
