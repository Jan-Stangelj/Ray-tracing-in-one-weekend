#pragma once

#include <glm/glm.hpp>
#include <optional>

namespace rt {
    struct hitInfo {
        glm::vec3 origin = glm::vec3(0.0f);
        std::optional<glm::vec3> normal = glm::vec3(0.0f);
        float d = 0.0f;
        bool hasHit = false;
        bool backface = false;
        uint32_t material = UINT32_MAX;
    };
}