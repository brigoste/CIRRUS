#include "solver/preconditioners/ILU0Preconditioner.hpp"
#include "equation_systems/LinearSystem.hpp"
#include "equation_systems/LinearEquationSystem.hpp"

#include <stdexcept>

void ILU0Preconditioner::copyMatrix(const LinearEquationSystem& sys)
{
    for (std::size_t i = 0; i < N_; ++i)
    {
        for (const auto& [j, aij] : sys.row(i))
        {
            LU_[i][j] = aij;
        }
    }
}

void ILU0Preconditioner::factorize()
{
    // Implement ILU(0) factorization
    for (std::size_t i = 0; i < N_; ++i)
    {
        for (auto& [j,aij] : LU_[i])
        {
            if (j >= i) 
            { 
                continue; 
            }

            auto diagIt = LU_[j].find(j);

            if (diagIt == LU_[j].end()) 
            { 
                throw std::runtime_error("ILU0: missing diagonal."); 
            }

            double diag = diagIt->second;

            if (std::abs(diag) < 1e-30) 
            {
                throw std::runtime_error("ILU0: zero pivot."); 
            }

            aij /= diag;

            for (auto [k,ujk] : LU_[j])
            {
                if (k <= j) 
                { 
                    continue; 
                }

                auto it = LU_[i].find(k);

                // ILU(0): only existing entries
                if (it != LU_[i].end()) 
                { 
                    it->second -= aij * ujk; 
                }
            }
        }
    }
}

void ILU0Preconditioner::setup(const LinearEquationSystem& sys)
{
    N_ = sys.size();
    LU_.assign(N_, {});     // guarantees an empty map over LU_.resize(N_);

    copyMatrix(sys);
    factorize();
}

void ILU0Preconditioner::apply( const std::vector<double>& r, std::vector<double>& z) const
{
    std::vector<double> y(N_,0.0);

    // Forward solve Ly=r
    for(std::size_t i = 0 ; i < N_; ++i)
    {
        double sum = r[i];

        for(auto [j,val] : LU_[i]) 
        { 
            if(j < i) { sum -= val*y[j]; } 
        }

        y[i] = sum;
    }

    // Backward solve Uz=y
    z.assign(N_,0.0);

    for (std::size_t ii = N_; ii-- > 0; )
    {
        double sum = y[ii];
        auto diagIt = LU_[ii].find(ii);

        if (diagIt == LU_[ii].end()) 
        { 
            throw std::runtime_error("ILU0: missing diagonal during solve."); 
        }

        double diag = diagIt->second;

        for (auto [j, val] : LU_[ii])
        {
            if (j > ii) { sum -= val * z[j]; }
        }

        z[ii] = sum / diag;
    }
}
