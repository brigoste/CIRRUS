#include "discretization/reconstructors/ReconstructionFactory.hpp"
#include "discretization/reconstructors/GradientReconstruction.hpp"
#include "discretization/reconstructors/CentralReconstruction.hpp"
#include "discretization/reconstructors/UpwindReconstruction.hpp"
#include "discretization/reconstructors/SecondOrderUpwindReconstruction.hpp"
#include "discretization/reconstructors/QuickReconstruction.hpp"
#include "discretization/reconstructors/MusclReconstruction.hpp"

#include <stdexcept>


std::unique_ptr<ReconstructionScheme> makeReconstructionScheme(ReconstructionType type)
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
    }

    throw std::runtime_error( "Unsupported reconstruction scheme." );
}
