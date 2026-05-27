#pragma once

#include <glm/glm.hpp>

namespace rt {
    class ray {
    public:

        ray(glm::vec3 origin, glm::vec3 direction) : m_origin(origin), m_direction(direction) {}
        ~ray() = default;

        glm::vec3 origin() { return m_origin; }
        glm::vec3 direction() {return m_direction; }

        glm::vec3 at(float distance) { return m_origin + distance*m_direction; }

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        glm::vec3 m_direction = glm::vec3(0.0f);

    };
}