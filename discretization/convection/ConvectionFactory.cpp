#include "discretization/convection/ConvectionFactory.hpp"

#include "discretization/convection/CentralDifferenceScheme.hpp"
#include "discretization/convection/UpwindScheme.hpp"

#include <stdexcept>

std::unique_ptr<ConvectionScheme> makeConvectionScheme(ConvectionType type)
{
    switch (type)
    {
        case ConvectionType::CentralLinear:
            return std::make_unique<CentralDifferenceScheme>();

        case ConvectionType::Upwind:
            return std::make_unique<UpwindScheme>();
    }

    throw std::runtime_error(
        "Unsupported convection scheme."
    );
}