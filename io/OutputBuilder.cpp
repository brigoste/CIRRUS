#include "io/OutputBuilder.hpp"

#include "io/OutputData.hpp"
#include "io/PointField.hpp"

#include "simulation/Simulation.hpp"
#include "fields/ScalarField.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"

#include "equation_systems/LinearEquationSystem.hpp"

#include <memory>

OutputData OutputBuilder::build(
    const Simulation& sim,
    const ScalarField& field,
    const std::vector<double>& residual)
{
    auto reconstructed = std::make_shared<PointField>( BoundaryReconstructor::reconstruct( sim.mesh(), sim.boundary(), sim.model(), field));
    
    const auto& sys = sim.system();

    std::vector<double> rhs(sys.size());

    for (std::size_t i = 0; i < sys.size(); ++i)
    {
        rhs[i] = sys.rhs(i);
    }

    return OutputData{
        sim.mesh(),
        { FieldOutput{ field.name(), &field, nullptr } },
        reconstructed,
        std::move(rhs),
        residual
    };
}
