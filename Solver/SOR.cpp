#include "Solver/SOR.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

std::vector<double> SOR(
    LinearSystem& sys,
    int iter,
    double tol,
    double omega,
    bool output
)
{
    const int N = sys.aP.size();
    int max_iter = 0;

    if (omega <= 0.0 || omega >= 2.0)
    {
        std::cerr << "SOR: omega out of bounds (0,2)\n";
        return {};
    }

    std::fill(sys.x.begin(), sys.x.end(), 0.0);
    std::fill(sys.x_old.begin(), sys.x_old.end(), 0.0);

    for (int it = 0; it < iter; ++it)
    {
        double maxDiff = 0.0;

        for (int i = 0; i < N; ++i)
        {
            const double aP = sys.aP[i];

            if (std::abs(aP) < 1e-14)
            {
                std::cerr << "SOR: zero diagonal at i=" << i << "\n";
                return {};
            }

            const double old = sys.x[i];

            double rhs = sys.b[i];

            if (i > 0)
                rhs -= sys.aW[i] * sys.x[i - 1];

            if (i < N - 1)
                rhs -= sys.aE[i] * sys.x_old[i + 1];

            // Gauss-Seidel update
            const double gs = rhs / aP;

            // SOR relaxation
            sys.x[i] = (1.0 - omega) * old + omega * gs;

            maxDiff = std::max(maxDiff, std::abs(sys.x[i] - old));

            sys.x_old[i] = sys.x[i];
        }
        max_iter++;

        if (maxDiff < tol)
            break;
    }
    
    if(output){
        if(max_iter < iter)
            std::cout << "Solution converged after " << max_iter << " iterations." << std::endl;
        else
            std::cout << "Maximum iterations" << std::endl;
    }
    
    return sys.x;
}
