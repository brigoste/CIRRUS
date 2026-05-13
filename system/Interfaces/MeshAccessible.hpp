#include "mesh\Mesh1D.hpp"

class MeshAccessible {
public:
    virtual ~MeshAccessible() = default;

    virtual const Mesh1D& mesh() const = 0;
};
