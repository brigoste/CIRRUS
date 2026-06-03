#include "postprocessing/DerivedFields.hpp"

#include <stdexcept>

std::vector<double>
DerivedFields::gradient1D(
    const MeshBase& mesh,
    const std::vector<double>& phi)
{
    const std::size_t n = phi.size();
    std::vector<double> grad(n, 0.0);

    if (n < 2)
        return grad;

    for (std::size_t i = 1; i < n - 1; ++i)
    {
        const double dx =
            mesh.cellCenter(i + 1)[0]
          - mesh.cellCenter(i - 1)[0];

        grad[i] =
            (phi[i + 1] - phi[i - 1]) / dx;
    }

    // simple boundary extrapolation
    grad[0]     = grad[1];
    grad[n - 1] = grad[n - 2];

    return grad;
}

std::vector<double>
DerivedFields::heatFlux1D(
    const MeshBase& mesh,
    const std::vector<double>& phi,
    double k)
{
    auto grad = gradient1D(mesh, phi);

    for (double& g : grad)
        g = -k * g;

    return grad;
}
