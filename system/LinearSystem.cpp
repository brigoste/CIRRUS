#include "LinearSystem.hpp"
#include <algorithm>
#include <functional>

LinearSystem::LinearSystem(int n)
    : aP(n, 0.0),
      aE(n, 0.0),
      aW(n, 0.0),
      b(n, 0.0),
      x(n, 0.0),
      x_old(n, 0.0),
      Su(n, 0.0),
      Sp(n, 0.0)
{}

void LinearSystem::reset()
{
    std::fill(aP.begin(), aP.end(), 0.0);
    std::fill(aE.begin(), aE.end(), 0.0);
    std::fill(aW.begin(), aW.end(), 0.0);
    std::fill(b.begin(),  b.end(),  0.0);
    std::fill(Su.begin(), Su.end(), 0.0);
    std::fill(Sp.begin(), Sp.end(), 0.0);
}

// ------------------------------------------------------------
// Proper member function
// ------------------------------------------------------------
void LinearSystem::setSource(
    std::function<double(double)> Su_func,
    std::function<double(double)> Sp_func,
    const std::vector<double>& x_coord
)
{
    const int n = static_cast<int>(x_coord.size());

    Su.resize(n);
    Sp.resize(n);

    for (int i = 0; i < n; ++i)
    {
        const double x = x_coord[i];

        Su[i] = Su_func ? Su_func(x) : 0.0;
        Sp[i] = Sp_func ? Sp_func(x) : 0.0;
    }
}
void LinearSystem::clearSource(const std::vector<double>& x)
{
    const int n = static_cast<int>(x.size());

    Su.resize(n);
    Sp.resize(n);

    for (int i = 0; i < n; ++i)
    {
        // const double x = x_coord[i];

        Su[i] = 0.0;
        Sp[i] = 0.0;
    }
}
