#pragma once

#include <cstddef>
#include <limits>

struct MeshTypes
{
    using index_type = std::size_t;

    static constexpr index_type INVALID = std::numeric_limits<index_type>::max();
};
