#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>

#include "Solver/SolverMethod.hpp"
#include "mesh/primitives/Point.hpp"
#include "mesh/BoundaryPatchSystem.hpp"

// -----------------------------
// Forward-safe definitions
// -----------------------------

struct BoundaryConfig
{
    std::size_t group;
    BoundaryPatchSystem::Condition condition;
};

struct MeshConfig
{
    std::string type = "line1d"; // "line1d", "quad2D"

    std::size_t nx = 0; // Needed for all dimensions (strucutred)
    std::size_t ny = 1; // only needed for 2+ dimensions (structred)

    double lx = 0.0;    // Needed for all dimensions (structured)
    double ly = 1.0;    // only needed for 2+ dimensions (structured)
};

struct PhysicsConfig
{
    std::string type;   // "heat", "navier-stokes", etc.

    double k = 0.0;
};
// Verification case
struct VerificationConfig
{
    bool enabled = false;

    // Registry key
    std::string case_name;

    struct
    {
        bool l2 = true;
        bool linf = true;
    } norms;

    struct
    {
        std::string csv;
        std::string summary;
    } output;
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

    VerificationConfig verification;
};



// factories
SimulationConfig defaultConfig();
SimulationConfig loadConfig(const std::filesystem::path& path);
