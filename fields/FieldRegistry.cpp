#include "fields/FieldRegistry.hpp"

#include <stdexcept>

ScalarField& FieldRegistry::createScalar(
    const std::string& name,
    const MeshBase& mesh,
    FieldLocation location,
    double initialValue
)
{
    if (contains(name)) { throw std::runtime_error("Field already exists: " + name); }

    auto field = std::make_unique<ScalarField>( name, mesh, location, initialValue );

    ScalarField& ref = *field;

    fields_.emplace( name, std::move(field) );

    return ref;
}

ScalarField& FieldRegistry::createScalar(
    FieldName name,
    const MeshBase& mesh,
    FieldLocation location,
    double initialValue
)
{
    if (name == FieldName::Invalid) { throw std::runtime_error( "Cannot create scalar field with invalid FieldName"); }

    return createScalar(
        std::string(to_string(name)),
        mesh,
        location,
        initialValue
    );
}

VectorField& FieldRegistry::createVector(
    const std::string& name,
    const MeshBase& mesh,
    FieldLocation location,
    const Vector& initialValue
)
{
    if (contains(name))
    {
        throw std::runtime_error(
            "Field already exists: " + name
        );
    }

    auto field = std::make_unique<VectorField>(
        name,
        mesh,
        location,
        initialValue
    );

    VectorField& ref = *field;

    fields_.emplace(
        name,
        std::move(field)
    );

    return ref;
}

VectorField& FieldRegistry::createVector(
    FieldName name,
    const MeshBase& mesh,
    FieldLocation location,
    const Vector& initialValue
)
{
    if (name == FieldName::Invalid)
    {
        throw std::runtime_error(
            "Cannot create vector field with invalid FieldName"
        );
    }

    return createVector(
        std::string(to_string(name)),
        mesh,
        location,
        initialValue
    );
}
bool FieldRegistry::contains( const std::string& name ) const { return fields_.find(name) != fields_.end(); }

FieldBase& FieldRegistry::get( const std::string& name )
{
    auto it = fields_.find(name);
    if (it == fields_.end()) { throw std::runtime_error( "Field not found: " + name ); }

    return *(it->second);
}

const FieldBase& FieldRegistry::get( const std::string& name ) const
{
    auto it = fields_.find(name);
    if (it == fields_.end()) { throw std::runtime_error( "Field not found: " + name ); }

    return *(it->second);
}

FieldBase& FieldRegistry::get(FieldName name)                             
{
    if (name == FieldName::Invalid) { throw std::runtime_error("Cannot access field with invalid FieldName"); } 
    return get(std::string(to_string(name))); 
}
const FieldBase& FieldRegistry::get(FieldName name) const                             
{
    if (name == FieldName::Invalid) { throw std::runtime_error("Cannot access field with invalid FieldName"); } 
    return get(std::string(to_string(name))); 
}

ScalarField& FieldRegistry::scalar( FieldName name )                      { return dynamic_cast<ScalarField&>( get(name) ); }
const ScalarField& FieldRegistry::scalar( FieldName name ) const          { return dynamic_cast<const ScalarField&>( get(name) ); }
ScalarField& FieldRegistry::scalar(const std::string& name)               { return dynamic_cast<ScalarField&>(get(name));}
const ScalarField& FieldRegistry::scalar(const std::string& name) const   { return dynamic_cast<const ScalarField&>(get(name)); }

VectorField& FieldRegistry::vector( FieldName name )                      { return dynamic_cast<VectorField&>( get(name) ); }
const VectorField& FieldRegistry::vector( FieldName name ) const          { return dynamic_cast<const VectorField&>( get(name) ); }
VectorField& FieldRegistry::vector( const std::string& name )             { return dynamic_cast<VectorField&>( get(name) ); }
const VectorField& FieldRegistry::vector( const std::string& name ) const { return dynamic_cast<const VectorField&>( get(name) ); }

void FieldRegistry::clear()                                               { fields_.clear(); }
std::size_t FieldRegistry::size() const                                   { return fields_.size(); }
