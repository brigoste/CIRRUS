#pragma once

#include "verification/qoi/QuantityOfInterest.hpp"
#include "mesh/primitives/Point.hpp"

class PointValueQoI : public QuantityOfInterest
{
public:
    explicit PointValueQoI(Point position);

    double evaluate(
        const ScalarField& field,
        const Interpolator& interpolator
    ) const override;

    const Point& position() const;

private:
    Point position_;
};