#include "discretization/reconstructors/ReconstructionFactory.hpp"
#include "discretization/reconstructors/basic/GradientReconstruction.hpp"
#include "discretization/reconstructors/basic/CentralReconstruction.hpp"
#include "discretization/reconstructors/basic/UpwindReconstruction.hpp"
#include "discretization/reconstructors/higher_order/SecondOrderUpwindReconstruction.hpp"
#include "discretization/reconstructors/higher_order/QuickReconstruction.hpp"
#include "discretization/reconstructors/higher_order/MusclReconstruction.hpp"
#include "discretization/reconstructors/tvd/TVDReconstruction.hpp"
#include "discretization/reconstructors/tvd/MinmodLimiter.hpp"

#include "discretization/reconstructors/tvd/FluxLimiterType.hpp"
#include "discretization/reconstructors/tvd/FluxLimiterFactory.hpp"

#include <stdexcept>

std::unique_ptr<ReconstructionScheme> makeReconstructionScheme(ReconstructionType type, FluxLimiterType limiterType)
{
    switch (type)
    {
        case ReconstructionType::Gradient:
            return std::make_unique<GradientReconstruction>();

        case ReconstructionType::Central:
            return std::make_unique<CentralReconstruction>();

        case ReconstructionType::Upwind:
            return std::make_unique<UpwindReconstruction>();
        
        case ReconstructionType::SecondOrderUpwind:
            return std::make_unique<SecondOrderUpwindReconstruction>();
        
        case ReconstructionType::QUICK:
            return std::make_unique<QuickReconstruction>();

        case ReconstructionType::MUSCL:
            return std::make_unique<MusclReconstruction>();

        case ReconstructionType::TVD:
            return std::make_unique<TVDReconstruction>( 
                makeLimiter(limiterType)                        // There is another way to do this, but I don't love it.
            );
    }

    throw std::runtime_error( "Unsupported reconstruction scheme." );
}
