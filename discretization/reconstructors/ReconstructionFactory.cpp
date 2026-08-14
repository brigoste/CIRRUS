#include "discretization/reconstructors/ReconstructionFactory.hpp"
#include "discretization/reconstructors/basic/GradientReconstruction.hpp"
#include "discretization/reconstructors/basic/CentralReconstruction.hpp"
#include "discretization/reconstructors/basic/UpwindReconstruction.hpp"
#include "discretization/reconstructors/basic/SecondOrderUpwindReconstruction.hpp"
#include "discretization/reconstructors/basic/QuickReconstruction.hpp"
#include "discretization/reconstructors/basic/MusclReconstruction.hpp"

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
