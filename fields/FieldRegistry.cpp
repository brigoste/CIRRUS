#include "fields/FieldRegistry.hpp"

#include <stdexcept>

ScalarField& FieldRegistry::createScalar(
    const std::string& name,
    const MeshBase& mesh,
    FieldLocation location,
    double initialValue
)
{
    if (contains(name))
    {
        throw std::runtime_error( "Field already exists: " + name );
    }

    auto field = std::make_unique<ScalarField>(
        name,
        mesh,
        location,
        initialValue
    );

    ScalarField& ref = *field;

    fields_.emplace(
        name,
        std::move(field)
    );

    return ref;
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
        throw std::runtime_error( "Field already exists: " + name );
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

bool FieldRegistry::contains( const std::string& name ) const { return fields_.find(name) != fields_.end(); }

FieldBase& FieldRegistry::get( const std::string& name )
{
    auto it = fields_.find(name);

    if (it == fields_.end())
    {
        throw std::runtime_error( "Field not found: " + name );
    }

    return *(it->second);
}

const FieldBase& FieldRegistry::get( const std::string& name ) const
{
    auto it = fields_.find(name);

    if (it == fields_.end())
    {
        throw std::runtime_error( "Field not found: " + name );
    }

    return *(it->second);
}

ScalarField& FieldRegistry::scalar( const std::string& name )
{
    return dynamic_cast<ScalarField&>( get(name) );
}

const ScalarField& FieldRegistry::scalar( const std::string& name ) const
{
    return dynamic_cast<const ScalarField&>( get(name) );
}

VectorField& FieldRegistry::vector( const std::string& name )
{
    return dynamic_cast<VectorField&>( get(name) );
}

const VectorField& FieldRegistry::vector( const std::string& name ) const
{
    return dynamic_cast<const VectorField&>( get(name) );
}

void FieldRegistry::clear() { fields_.clear(); }
std::size_t FieldRegistry::size() const { return fields_.size(); }
