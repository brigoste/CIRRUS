#include "VerificationCaseFactory.hpp"

#include "ManufacturedCases/1DLinear.hpp"
#include "ManufacturedCases/Quadratic1D.hpp"
#include "ManufacturedCases/Sinusoidal2D.hpp"
#include "ManufacturedCases/AdvectionDiffusion2D.hpp"

std::unique_ptr<VerificationCase> VerificationCaseFactory::create(
    const std::string& name,
    const nlohmann::json& params)
{
    if (name == "Linear1D") { return std::make_unique<OneDLinear>(params); }

    if (name == "Quadratic1D") { return std::make_unique<Quadratic1D>(params); }

    if (name == "Sinusoidal2D") { return std::make_unique<Sinusoidal2D>(params); }

    if (name == "AdvectionDiffusion2D") { return std::make_unique<AdvectionDiffusion2D>(params); }

    throw std::runtime_error("Unknown verification case: " + name);
}
