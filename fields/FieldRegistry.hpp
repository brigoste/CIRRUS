#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "fields/FieldBase.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

// FieldRegistry owns the data, not references. This is a larger memory bottleneck, but it does reflect on what
//      this class is: the owner of each field's data.

class FieldRegistry
{
public:

    FieldRegistry() = default;

    // Create fields
    ScalarField& createScalar(
        const std::string& name,
        const MeshBase& mesh,
        FieldLocation location,
        double initialValue = 0.0
    );

    VectorField& createVector(
        const std::string& name,
        const MeshBase& mesh,
        FieldLocation location,
        const Vector& initialValue = Vector{}
    );


    // Access fields
    ScalarField& scalar(const std::string& name);
    const ScalarField& scalar(const std::string& name) const;

    VectorField& vector(const std::string& name);
    const VectorField& vector(const std::string& name) const;


    // General access
    bool contains(const std::string& name) const;

    FieldBase& get(const std::string& name);
    const FieldBase& get(const std::string& name) const;


    // Management
    void clear();

    std::size_t size() const;


private:

    std::unordered_map<
        std::string,
        std::unique_ptr<FieldBase>
    > fields_;
};
