#include "VerificationCaseFactory.hpp"

#include "ManufacturedCases/1DLinear.hpp"
#include "ManufacturedCases/Quadratic1D.hpp"
#include "ManufacturedCases/Sinusoidal2D.hpp"

std::unique_ptr<VerificationCase>
VerificationCaseFactory::create(
    const std::string& name)
{
    if (name == "Linear1D"){
        return std::make_unique<OneDLinear>();
    }

    if (name == "Quadratic1D"){
        return std::make_unique<Quadratic1D>();
    }

    if (name == "Sinusoidal2D"){
        return std::make_unique<Sinusoidal2D>();
    }

    throw std::runtime_error("Unknown verification case: " + name);
}
