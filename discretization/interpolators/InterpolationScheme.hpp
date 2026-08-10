#pragma once

struct InterpolationWeights
{
    double owner = 0.0;
    double neighbor = 0.0;
};

class InterpolationScheme
{
    public:
        virtual ~InterpolationScheme() = default;
        virtual InterpolationWeights interpolate( double alpha, double flux ) const = 0;
};