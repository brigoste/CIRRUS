#include "linear_system/LinearSystem.hpp"
#include <vector>
#include <cmath>
#include <stdexcept>

std::vector<double> GaussSeidel( const LinearSystem& sys, int max_iter, double tol)
{
    const std::size_t N = sys.size();

    std::vector<double> x(N, 0.0);

    for (int iter = 0; iter < max_iter; ++iter)
    {
        double maxRes = 0.0;

        for (std::size_t i = 0; i < N; ++i)
        {
            const auto& row = sys.row(i);

            double diag = 0.0;
            double sum = 0.0;

            for (const auto& [j, aij] : row)
            {
                if (j == i) 
                { 
                    diag = aij; 
                }
                else 
                { 
                    sum += aij * x[j]; 
                }
            }

            if (std::abs(diag) < 1e-14) 
            { 
                throw std::runtime_error("Zero diagonal in GS"); 
            }

            double x_new = (sys.rhs(i) - sum) / diag;

            maxRes = std::max(maxRes, std::abs(x_new - x[i]));
            x[i] = x_new;
        }

        if (maxRes < tol) 
        { 
            break; 
        }
    }

    return x;
}
