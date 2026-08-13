#include "discretization/reconstructors/ReconstructionFactory.hpp"
#include "discretization/reconstructors/GradientReconstruction.hpp"
#include "discretization/reconstructors/CentralReconstruction.hpp"
#include "discretization/reconstructors/UpwindReconstruction.hpp"
#include "discretization/reconstructors/SecondOrderUpwindReconstruction.hpp"

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
    }

    throw std::runtime_error( "Unsupported reconstruction scheme." );
}
