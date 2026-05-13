#include "linear_system\LinearSystem.hpp"

class MatrixBasedSystem {
public:
    virtual ~MatrixBasedSystem() = default;

    virtual const LinearSystem& matrix() const = 0;
};
