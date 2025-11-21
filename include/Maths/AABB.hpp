#pragma once

#include <glm/glm.hpp>

namespace Lexvi {
    struct AABB_2D {
        glm::vec2 min;
        glm::vec2 max;
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };
}