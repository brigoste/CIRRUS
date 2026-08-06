#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "fields/FieldBase.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "fields/FieldNames.hpp"

class FieldRegistry
{
public:

    FieldRegistry() = default;

    // Create fields
    ScalarField& createScalar( const std::string& name, const MeshBase& mesh, FieldLocation location, double initialValue);
    ScalarField& createScalar( const FieldName name, const MeshBase& mesh, FieldLocation location, double initialValue);

    VectorField& createVector( const std::string& name, const MeshBase& mesh, FieldLocation location, const Vector& initialValue = Vector{}  );
    VectorField& createVector( const FieldName name, const MeshBase& mesh, FieldLocation location, const Vector& initialValue = Vector{}  );

    // Access fields
    ScalarField& scalar(const std::string& name);
    const ScalarField& scalar(const std::string& name) const;
    ScalarField& scalar(FieldName name);
    const ScalarField& scalar(FieldName name) const;
    

    VectorField& vector(const std::string& name);
    const VectorField& vector(const std::string& name) const;
    VectorField& vector(FieldName name);
    const VectorField& vector(FieldName name) const;

    // General access
    bool contains(const std::string& name) const;

    FieldBase& get(const std::string& name);
    FieldBase& get(FieldName name);
    const FieldBase& get(const std::string& name) const;
    const FieldBase& get(FieldName name) const;

    // Management
    void clear();

    std::size_t size() const;


private:

    std::unordered_map< std::string, std::unique_ptr<FieldBase> > fields_;
};
