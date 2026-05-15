#include "config/SimulationConfigLoader.hpp"
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

static BoundaryFace parseFace(const std::string& s)
{
    if (s == "Left") return BoundaryFace::Left;
    if (s == "Right") return BoundaryFace::Right;
    if (s == "Top") return BoundaryFace::Top;
    if (s == "Bottom") return BoundaryFace::Bottom;

    throw std::runtime_error("Invalid boundary face: " + s);
}

static BCType parseBCType(const std::string& s)
{
    if (s == "Dirichlet") return BCType::Dirichlet;
    if (s == "Neumann") return BCType::Neumann;
    if (s == "Convective") return BCType::Convective;

    throw std::runtime_error("Invalid BC type: " + s);
}

SimulationConfig loadConfig(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file");

    json j;
    file >> j;

    SimulationConfig cfg;

    // ---------------- Mesh ----------------
    cfg.mesh.type = j["mesh"]["type"];
    cfg.mesh.n = j["mesh"]["n"];
    cfg.mesh.L = j["mesh"]["L"];

    // -------------- Physics --------------
    cfg.physics.k = j["physics"]["k"];
    cfg.physics.A = j["physics"]["A"];

    // -------------- Source --------------
    cfg.source.Su = j["source"]["Su"];
    cfg.source.Sp = j["source"]["Sp"];

    // -------------- Solver --------------
    cfg.solver.type = j["solver"]["type"];
    cfg.solver.tol = j["solver"]["tol"];
    cfg.solver.max_iter = j["solver"]["max_iter"];
    cfg.solver.omega = j["solver"]["omega"];

    // -------------- BCs ------------------
    for (auto& bc : j["boundary_conditions"])
    {
        BoundaryConditionDescriptor d;

        d.face = parseFace(bc["face"]);
        d.type = parseBCType(bc["type"]);

        if (d.type == BCType::Dirichlet)
            d.value = bc["value"];

        else if (d.type == BCType::Neumann)
            d.flux = bc["flux"];

        else if (d.type == BCType::Convective)
        {
            d.h = bc["h"];
            d.Tinf = bc["Tinf"];
        }

        cfg.bcs.push_back(d);
    }

    return cfg;
}
