#pragma once

#include <glm/glm.hpp>
#include <string>

namespace rt {
    struct hitInfo {
        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        float d = 0.0f;
        bool hasHit = false;
        bool backface = false;
        std::string material = "";
    };
}