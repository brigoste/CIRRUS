#pragma once

#include <string_view>

enum class FieldName
{
    Invalid,

    Temperature,
    Pressure,
    VelocityX,
    VelocityY,
    VelocityZ
};


inline std::string_view to_string(FieldName name)
{
    switch(name)
    {
        case FieldName::Invalid:
            return "Invalid";

        case FieldName::Temperature:
            return "Temperature";

        case FieldName::Pressure:
            return "Pressure";

        case FieldName::VelocityX:
            return "VelocityX";

        case FieldName::VelocityY:
            return "VelocityY";

        case FieldName::VelocityZ:
            return "VelocityZ";
    }

    return "Invalid";
}
