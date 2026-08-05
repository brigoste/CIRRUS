#pragma once

#include <vector>
#include <string>

#include "fields/FieldBase.hpp"
#include "fields/FieldLocation.hpp"
#include "mesh/primitives/Vector.hpp"


class VectorField : public FieldBase
{
public:

    VectorField(
        std::string name,
        const MeshBase& mesh,
        FieldLocation location,
        const Vector& initialValue = Vector()
    );

    std::size_t size() const override;

    Vector& operator[](std::size_t i);
    const Vector& operator[](std::size_t i) const;

    void fill(const Vector& value);


private:

    std::vector<Vector> values_;
};
