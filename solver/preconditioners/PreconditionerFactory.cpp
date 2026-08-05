#include "solver/preconditioners/PreconditionerFactory.hpp"
#include "solver/preconditioners/IdentityPreconditioner.hpp"
#include "solver/preconditioners/JacobiPreconditioner.hpp"
#include "solver/preconditioners/ILU0Preconditioner.hpp"

#include <stdexcept>

std::unique_ptr<Preconditioner> createPreconditioner(PreconditionerType type)
{
    switch(type)
    {
        case PreconditionerType::None:
            return std::make_unique<IdentityPreconditioner>();

        case PreconditionerType::Jacobi:
            return std::make_unique<JacobiPreconditioner>();

        case PreconditionerType::ILU0:
            return std::make_unique<ILU0Preconditioner>();
    }

    throw std::runtime_error("Unknown preconditioner");
}
