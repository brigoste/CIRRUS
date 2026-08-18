#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <nlohmann/json.hpp>
#include "solver/preconditioners/PreconditionerType.hpp"

class LinearEquationSystem;

inline PreconditionerType preconditioner_from_string(const std::string& s)
{
    if (s == "None")   
    {
        return PreconditionerType::None;
    }
    if (s == "Jacobi") 
    {
        return PreconditionerType::Jacobi;
    }
    if (s == "ILU0")   
    {
        return PreconditionerType::ILU0;
    }

    if (s == "SSOR")   
    {
        return PreconditionerType::SSOR;
    }

    throw std::runtime_error( "Unknown preconditioner: " + s);
}


inline void from_json(
    const nlohmann::json& j,
    PreconditionerType& p)
{
    p = preconditioner_from_string( j.get<std::string>());
}

inline void to_json(
    nlohmann::json& j,
    const PreconditionerType& p)
{
    switch (p)
    {
        case PreconditionerType::None:
            j = "None";
            break;

        case PreconditionerType::Jacobi:
            j = "Jacobi";
            break;

        case PreconditionerType::ILU0:
            j = "ILU0";
            break;

        case PreconditionerType::SSOR:
            j = "SSOR";
            break;
    }
}

class Preconditioner
{
public:

    virtual ~Preconditioner() = default;

    virtual void setup(
        const LinearEquationSystem& sys) = 0;

    virtual void apply(
        const std::vector<double>& r,
        std::vector<double>& z) const = 0;

    virtual std::string name() const = 0;
};
