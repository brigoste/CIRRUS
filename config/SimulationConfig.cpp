#include "config/SimulationConfig.hpp"
// #include "mesh/primitives/Point.hpp"

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
    cfg.mesh.nx = 50;
    cfg.mesh.lx = 1.0;

    // -------------------------
    // Physics defaults
    // -------------------------
    cfg.physics.k = 1.0;

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
    cfg.verification = VerificationConfig{};

    // -------------------------
    // Mesh
    // -------------------------
    cfg.mesh.type = j.at("mesh").at("type").get<std::string>();
    cfg.mesh.nx = j.at("mesh").at("nx").get<std::size_t>();
    cfg.mesh.ny = j.at("mesh").value("ny", 1);
    cfg.mesh.lx = j.at("mesh").at("lx").get<double>();
    cfg.mesh.ly = j.at("mesh").value("ly", 1.0);

    // -------------------------
    // Physics
    // -------------------------
    cfg.physics.type = j.at("physics").at("type").get<std::string>();
    cfg.physics.k = j.at("physics").at("k").get<double>();
    cfg.physics.Su = j.at("physics").at("Su").get<double>();
    cfg.physics.Sp = j.at("physics").at("Sp").get<double>();

    std::cout
    << "Config Su = " << cfg.physics.Su
    << "\n"
    << "Config Sp = " << cfg.physics.Sp
    << "\n";

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
        // const std::string f =
        //     bcJson.at("patch").get<std::string>();

        const std::size_t group = bcJson.at("group").get<std::size_t>();

        bc.group = group;

        // -------------------------
        // BC type
        // -------------------------
        bc.condition.type = bc::from_string(bcJson.at("type").get<std::string>());

        switch (bc.condition.type)
        {
            case bc::Type::Dirichlet:
                bc.condition.value = bcJson.at("value").get<double>();
                break;

            case bc::Type::Neumann:
                bc.condition.flux = bcJson.at("flux").get<double>();
                break;

            case bc::Type::Convective:
                bc.condition.h = bcJson.at("h").get<double>();
                bc.condition.Tinf = bcJson.at("Tinf").get<double>();
                break;

            default:
                break;
        }

        cfg.boundary.push_back(bc);
    }

    if (j.contains("verification"))
    {
        const auto& v = j.at("verification");

        cfg.verification.enabled = v.value("enabled", false);

        cfg.verification.case_name = v.value("case_name", "");

        if (v.contains("norms"))
        {
            const auto& n = v.at("norms");

            cfg.verification.norms.l2 = n.value("l2", true);

            cfg.verification.norms.linf = n.value("linf", true);
        }

        if (v.contains("output"))
        {
            const auto& o = v.at("output");

            cfg.verification.output.csv =
                o.value("csv", "verification.csv");

            cfg.verification.output.summary =
                o.value("summary", "verification.json");
        }

        // ------------------------------------
        // Redirect verification outputs
        // ------------------------------------
        if (cfg.verification.enabled)
        {
            if (cfg.verification.output.csv == "verification.csv")
            {
                cfg.verification.output.csv = "output/validation/verification.csv";
            }

            if (cfg.verification.output.summary == "verification.json")
            {
                cfg.verification.output.summary = "output/validation/verification.json";
            }
        }
    }

    return cfg;
}
