#include "config/SimulationConfig.hpp"
#include "mesh/Point.hpp"

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================
// Default config
// ============================================================

SimulationConfig defaultConfig()
{
    SimulationConfig cfg;

    // -------------------------
    // Mesh defaults
    // -------------------------
    cfg.mesh.type = "1D";
    cfg.mesh.n = 50;
    cfg.mesh.L = 1.0;

    // -------------------------
    // Physics defaults
    // -------------------------
    cfg.physics.k = 1.0;

    // -------------------------
    // Source defaults
    // -------------------------
    cfg.source.Su = [](const Point&) { return 0.0; };
    cfg.source.Sp = [](const Point&) { return 0.0; };

    // -------------------------
    // Solver defaults
    // -------------------------
    cfg.solver.method = solver::Method::SOR;   // or TDMA depending on your preference
    cfg.solver.tol = 1e-8;
    cfg.solver.max_iter = 5000;
    cfg.solver.omega = 1.0;

    return cfg;
}

// ============================================================
// JSON loader
// ============================================================

SimulationConfig loadConfig(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file");

    json j;
    file >> j;

    SimulationConfig cfg = defaultConfig();

    // -------------------------
    // Mesh
    // -------------------------
    cfg.mesh.type = j.at("mesh").at("type").get<std::string>();
    cfg.mesh.n    = j.at("mesh").at("n").get<int>();
    cfg.mesh.L    = j.at("mesh").at("L").get<double>();

    // -------------------------
    // Physics
    // -------------------------
    cfg.physics.k = j.at("physics").at("k").get<double>();

    // -------------------------
    // Source
    // -------------------------
    SourceConfig source;
    const double Su0 = j.at("source").at("Su").get<double>();
    const double Sp0 = j.at("source").at("Sp").get<double>();

    cfg.source.Su = [Su0](const Point&) { return Su0; };
    cfg.source.Sp = [Sp0](const Point&) { return Sp0; };

    // -------------------------
    // Solver
    // -------------------------
    cfg.solver.method   = j.at("solver").at("type").get<solver::Method>();
    cfg.solver.tol      = j.at("solver").at("tol").get<double>();
    cfg.solver.max_iter = j.at("solver").at("max_iter").get<int>();
    cfg.solver.omega    = j.at("solver").at("omega").get<double>();

    // -------------------------
    // Boundary Conditions
    // -------------------------
    cfg.boundary.clear();

    for (const auto& bcJson : j.at("boundary_conditions"))
    {
        BoundaryConfig bc{};

        // Initialize condition fields
        bc.condition.value = 0.0;
        bc.condition.flux  = 0.0;
        bc.condition.h     = 0.0;
        bc.condition.Tinf  = 0.0;

        // -------------------------
        // Face mapping
        // -------------------------
        const std::string face =
            bcJson.at("face").get<std::string>();

        if (face == "LEFT")
        {
            bc.faceIndex = 0;
        }
        else if (face == "RIGHT")
        {
            // 1D mesh:
            // n cells -> n+1 faces
            // right boundary face = n
            bc.faceIndex = cfg.mesh.n;
        }
        else
        {
            throw std::runtime_error(
                "Unknown boundary face: " + face);
        }

        // -------------------------
        // BC type
        // -------------------------
        bc.condition.type = bc::from_string(bcJson.at("type").get<std::string>());

        switch (bc.condition.type)
        {
            case bc::Type::Dirichlet:
                bc.condition.value =
                    bcJson.at("value").get<double>();
                break;

            case bc::Type::Neumann:
                bc.condition.flux =
                    bcJson.at("flux").get<double>();
                break;

            case bc::Type::Convective:
                bc.condition.h =
                    bcJson.at("h").get<double>();
                bc.condition.Tinf =
                    bcJson.at("Tinf").get<double>();
                break;

            default:
                break;
        }

        cfg.boundary.push_back(bc);
    }

    // -------------------------
    // Debug output
    // -------------------------
    // std::cout
    //     << "Loaded "
    //     << cfg.boundary.size()
    //     << " boundary conditions\n";

    // for (const auto& bc : cfg.boundary)
    // {
    //     std::cout
    //         << "BC faceIndex = "
    //         << bc.faceIndex
    //         << "\n";
    // }

    return cfg;
}
