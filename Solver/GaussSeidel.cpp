#include "Solver/GaussSeidel.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

std::vector<double> GaussSeidel(
    LinearSystem& sys,
    int iter,
    double tol,
    bool output
)
{
    const int N = sys.aP.size();

    std::fill(sys.x.begin(), sys.x.end(), 0.0);
    std::fill(sys.x_old.begin(), sys.x_old.end(), 0.0);
    int n_iter = 0;

    for (int it = 0; it < iter; ++it)
    {
        double maxDiff = 0.0;
        n_iter++;

        for (int i = 0; i < N; ++i)
        {
            const double aP = sys.aP[i];

            if (std::abs(aP) < 1e-14)
            {
                std::cerr << "GaussSeidel: zero diagonal at i=" << i << "\n";
                return {};
            }

            const double old = sys.x[i];

            double rhs = sys.b[i];

            if (i > 0)
                rhs -= sys.aW[i] * sys.x[i - 1];

            if (i < N - 1)
                rhs -= sys.aE[i] * sys.x_old[i + 1];

            sys.x[i] = rhs / aP;

            maxDiff = std::max(maxDiff, std::abs(sys.x[i] - old));

            sys.x_old[i] = sys.x[i]; // keep sync
        }

        if (maxDiff < tol){
            if(output)
                std::cout << "Hit tolerance." << std::endl;
            break;
        }
    }
    if(output){
        if(n_iter < iter){
            std::cout << "Exited after " << n_iter << " iterations." << std::endl; 
        }
        else{
            std::cout << "Exited after maximum iterations." << std::endl; 
        }
    }

    return sys.x;
}
