#pragma once

#include "fields/ScalarField.hpp"
#include "interpolators/Interpolator.hpp"

class QuantityOfInterest
{
public:
    virtual ~QuantityOfInterest() = default;

    virtual double evaluate(
        const ScalarField& field,
        const Interpolator& interpolator
    ) const = 0;
};