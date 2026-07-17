#pragma once

#include <glm/glm.hpp>

namespace rt {
    struct hitInfo {
        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        float d = 0.0f;
        bool hasHit = false;
        bool backface = false;
        uint32_t material = UINT32_MAX;
    };
}