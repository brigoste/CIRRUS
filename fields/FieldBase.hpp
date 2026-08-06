#pragma once

#include <string>
#include <cstddef>

#include "mesh/MeshBase.hpp"
#include "fields/FieldLocation.hpp"

// Storage container blueprint. From here we want to derive:
//  1.) Vector Fields (velocity, flux, etc)
//  2.) Scalar Fields (Temperature, Pressure, etc.)

class FieldBase
{
public:
    virtual ~FieldBase() = default;

    const std::string& name() const 
    { 
        return name_; 
    }
    const MeshBase& mesh() const 
    { 
        return mesh_; 
    }
    FieldLocation location() const 
    { 
        return location_; 
    }

    virtual std::size_t size() const = 0;

protected:
    FieldBase(
        std::string name,
        const MeshBase& mesh,
        FieldLocation location);

    const MeshBase& mesh_;
    std::string name_;
    FieldLocation location_;
};
