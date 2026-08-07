#include "io/OutputBuilder.hpp"

#include "io/OutputData.hpp"
#include "io/PointField.hpp"

#include "simulation/Simulation.hpp"
#include "fields/ScalarField.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"

#include <vector>
#include <memory>

OutputData OutputBuilder::build(
    const Simulation& sim,
    const ScalarField& field)
{
    auto reconstructed = 
    std::make_shared<PointField>(
        BoundaryReconstructor::reconstruct(
            sim.mesh(),
            sim.boundary(),
            sim.model(),
            field));

    std::vector<FieldOutput> fields;

    fields.push_back(
        FieldOutput{
            field.name(),
            &field,
            nullptr
        });

    fields.push_back(
        FieldOutput{
            "reconstructed",
            nullptr,
            reconstructed.get()
        });

    std::vector<double> residual(
        field.size(),
        0.0);

    return OutputData{
        sim.mesh(),
        {
            FieldOutput{
                field.name(),
                &field,
                nullptr
            }
        },
        reconstructed,
        sim.system().RHS(),
        residual
    };
}