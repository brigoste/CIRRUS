#pragma once

#include <vector>
#include <cstddef>

#include "fields/FieldBase.hpp"

class ScalarField : public FieldBase
{
public:

    ScalarField(
        std::string name,
        const MeshBase& mesh,
        FieldLocation location,
        double initialValue = 0.0
    );

    // value access
    double& operator[](std::size_t i);
    const double& operator[](std::size_t i) const;

    // FieldBase interface
    std::size_t size() const override;

    // Utility
    void fill(double value);
    const std::vector<double>& values() const { return values_; }

private:

    std::vector<double> values_;
};
