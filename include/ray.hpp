#pragma once

#include <glm/glm.hpp>

namespace rt {
    class ray {
    public:

        ray(glm::vec3 origin, glm::vec3 direction);
        ~ray() = default;

        glm::vec3 origin() const;
        glm::vec3 direction() const;

        glm::vec3 at(float distance) const;

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        glm::vec3 m_direction = glm::vec3(0.0f);

    };
}