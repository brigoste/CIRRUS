#pragma once

class FluxLimiter
{
public:

    virtual ~FluxLimiter() = default;

    /**
     * Evaluate the flux-limiter function.
     *
     * Parameters:
     *   r = ratio of consecutive solution gradients.
     *
     * Returns:
     *   Limiter value psi(r).
     */
    virtual double limit(double r) const = 0;
};