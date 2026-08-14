#include "VerificationCaseFactory.hpp"

#include "ManufacturedCases/1DLinear.hpp"
#include "ManufacturedCases/Quadratic1D.hpp"
#include "ManufacturedCases/Sinusoidal2D.hpp"
#include "ManufacturedCases/AdvectionDiffusion2D.hpp"
#include "ManufacturedCases/Neumann1D.hpp"
#include "ManufacturedCases/Neumann2D.hpp"
#include "ManufacturedCases/Robin1D.hpp"
#include "ManufacturedCases/Robin2D.hpp"

std::unique_ptr<VerificationCase> VerificationCaseFactory::create(
    const std::string& name,
    const SimulationConfig& config)
{
    if (name == "Linear1D") { return std::make_unique<OneDLinear>(); }

    if (name == "Quadratic1D") { return std::make_unique<Quadratic1D>(config); }

    if (name == "Sinusoidal2D") { return std::make_unique<Sinusoidal2D>(config); }

    if (name == "AdvectionDiffusion2D") { return std::make_unique<AdvectionDiffusion2D>(config); }

    if (name == "Neumann1D") { return std::make_unique<Neumann1D>(config); }

    if (name == "Neumann2D") { return std::make_unique<Neumann2D>(config); }

    if (name == "Robin1D") { return std::make_unique<Robin1D>(config); }

    if (name == "Robin2D") { return std::make_unique<Robin2D>(config); }

    throw std::runtime_error("Unknown verification case: " + name);
}
