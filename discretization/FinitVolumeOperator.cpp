#include "discretization/FiniteVolumeOperator.hpp"
#include "mesh/Face.hpp"
#include <limits>
#include <vector>
#include <cmath>
#include <stdexcept>

// static constexpr std::size_t INVALID = std::numeric_limits<std::size_t>::max();

// static double distance(const Point& a, const Point& b)
// {
//     double dx = a.x[0] - b.x[0];
//     double dy = a.x[1] - b.x[1];
//     return std::sqrt(dx*dx + dy*dy);
// }

#include "discretization/FiniteVolumeOperator.hpp"
#include <stdexcept>

void FiniteVolumeOperator::assemble(
    const MeshBase& mesh,
    const HeatEquationModel& model,
    LinearSystem& sys)
{
    const std::size_t N = mesh.ncells();
    const auto INVALID = std::numeric_limits<std::size_t>::max();

    std::vector<double> ap(N, 0.0);

    for (auto it = mesh.facesBegin(); it != mesh.facesEnd(); ++it)
    {
        const Face& f = *it;

        const double a = model.k * f.area / f.centroidDistance;

        const std::size_t o = f.owner;
        const std::size_t n = f.neighbor;

        if (n != INVALID)
        {
            sys.addCoeff(o, n, -a);
            sys.addCoeff(n, o, -a);

            ap[o] += a;
            ap[n] += a;
        }
        else
        {
            switch (f.bcType)
            {
                case BCType::Dirichlet:
                    ap[o] += a;
                    sys.addRHS(o, a * f.value);
                    break;

                case BCType::Neumann:
                    sys.addRHS(o, f.flux * f.area);
                    break;

                case BCType::Convective:
                {
                    double hA = f.h * f.area;
                    ap[o] += hA;
                    sys.addRHS(o, hA * f.Tinf);
                    break;
                }
            }
        }
    }

    for (std::size_t c = 0; c < N; ++c)
        sys.addDiag(c, ap[c]);
}
