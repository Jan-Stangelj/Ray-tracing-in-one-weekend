#pragma once

#include <glm/glm.hpp>

#include "ray.hpp"

#include <cstdint>

namespace rt {
    class camera {
    public:

        camera(glm::vec3 origin, glm::vec3 lookAt, uint32_t resolutionX, uint32_t resolutionY, float fovY = 60.0f);
        camera() = default;
        ~camera() = default;

        uint32_t resolutionX() const;
        uint32_t resolutionY() const;

        rt::ray genRay(uint32_t x, uint32_t y) const;

    private:

        glm::mat4 m_vpInv = glm::mat4(1.0f);

        glm::vec3 m_origin = glm::vec3(0.0f);
        glm::vec3 m_lookAt = glm::vec3(0.0f);

    };
}