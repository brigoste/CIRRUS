#pragma once

#include "discretization/gradient/GradientScheme.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/MeshBase.hpp"
#include <memory>

class GradientOperator
{
public:

    explicit GradientOperator(
        std::unique_ptr<GradientScheme> scheme
    );

    void compute(
        const MeshBase& mesh,
        const ScalarField& field,
        VectorField& gradient
    ) const;


private:

    std::unique_ptr<GradientScheme> scheme_;

};