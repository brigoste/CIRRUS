#include <stdexcept>
#include <string>

namespace physics{
    enum class PhysicsType
    {
        Heat,
        AdvectionDiffusion
    };

    inline PhysicsType physicsFromString(const std::string& s)
    {
        if (s == "heat") return PhysicsType::Heat;
        if (s == "advection-diffusion") return PhysicsType::AdvectionDiffusion;

        throw std::runtime_error("Unknown physics type: " + s);
    }

    inline const char* to_string(PhysicsType t)
    {
        switch (t)
        {
            case PhysicsType::Heat: return "heat";
            case PhysicsType::AdvectionDiffusion:   return "advection-diffusion";
        }

        return "UNKNOWN";
    }
}
