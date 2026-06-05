#pragma once

#include <glm/glm.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "ray.hpp"

#include <cstdint>

namespace rt {
    class camera {
    public:

        camera(glm::vec3 origin, glm::vec3 lookAt, uint32_t resolutionX, uint32_t resolutionY, float fovY = 60.0f) : m_origin(origin), 
                                                                                                                     m_lookAt(lookAt), 
                                                                                                                     m_resolutionX(resolutionX), 
                                                                                                                     m_resolutionY(resolutionY), 
                                                                                                                     m_fovY(fovY) {

            glm::mat4 view = glm::lookAt(origin, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 proj = glm::perspective(glm::radians(fovY), (float)resolutionX/(float)resolutionY, 0.1f, 100.0f);
            glm::mat4 vp = proj * view;
            m_vpInv = glm::inverse(vp);
        }
        ~camera() = default;

        uint32_t resolutionX() const { return m_resolutionX; }
        uint32_t resolutionY() const { return m_resolutionY; }

        rt::ray genRay(uint32_t x, uint32_t y) const {
            float ndcX = (x + 0.5f) / m_resolutionX * 2 - 1;
            float ndcY = ((m_resolutionY - y) + 0.5f) / m_resolutionY * 2 - 1;

            glm::vec4 nearClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
            glm::vec4 farClip = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

            glm::vec4 nearWorld = m_vpInv * nearClip;
            glm::vec4 farWorld = m_vpInv * farClip;

            nearWorld /= nearWorld.w;
            farWorld /= farWorld.w;

            return rt::ray(glm::vec3(nearWorld), glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld)));
        }

    private:

        glm::mat4 m_vpInv = glm::mat4(1.0f);

        glm::vec3 m_origin = glm::vec4(0.0f);
        glm::vec3 m_lookAt = glm::vec4(0.0f);

        uint32_t m_resolutionX = 800;
        uint32_t m_resolutionY = 600;

        float m_fovY = 60.0f;

    };
}