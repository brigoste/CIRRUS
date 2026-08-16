#include "verification/qoi/PointValueQoI.hpp"

PointValueQoI::PointValueQoI(Point position)
    :
    position_(position)
{
}

double PointValueQoI::evaluate(
    const ScalarField& field,
    const Interpolator& interpolator
) const
{
    return interpolator.interpolate(
        field,
        position_
    );
}

const Point& PointValueQoI::position() const
{
    return position_;
}