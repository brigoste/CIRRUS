#include "discretization/reconstructors/tvd/FluxLimiter.hpp"
#include "discretization/reconstructors/tvd/FluxLimiterType.hpp"
#include "discretization/reconstructors/tvd/MinmodLimiter.hpp"
#include "discretization/reconstructors/tvd/SuperbeeLimiter.hpp"
#include "discretization/reconstructors/tvd/VanLeerLimiter.hpp"
#include "discretization/reconstructors/tvd/MCLimiter.hpp"
#include "discretization/reconstructors/tvd/VanAlbadaLimiter.hpp"

#include <memory>
#include <stdexcept>

std::unique_ptr<FluxLimiter> makeLimiter(FluxLimiterType type)
{
    switch (type)
    {
        case FluxLimiterType::Minmod:
            return std::make_unique<MinmodLimiter>();

        case FluxLimiterType::Superbee:
            return std::make_unique<SuperbeeLimiter>();
        
        case FluxLimiterType::VanLeer:
            return std::make_unique<VanLeerLimiter>();

        case FluxLimiterType::MC:
            return std::make_unique<MCLimiter>();  
        
        case FluxLimiterType::VanAlbada:
            return std::make_unique<VanAlbadaLimiter>();  
    }

    throw std::runtime_error("Unsupported limiter.");
}
