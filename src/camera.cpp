#include "camera.hpp"

#include "settings.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace rt {

    camera::camera(glm::vec3 origin, glm::vec3 lookAt, uint32_t resolutionX, uint32_t resolutionY, float fovY) : m_origin(origin), 
                                                                                                                 m_lookAt(lookAt) {

        glm::mat4 view = glm::lookAt(origin, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(fovY), (float)resolutionX/(float)resolutionY, 0.1f, 100.0f);
        glm::mat4 vp = proj * view;
        m_vpInv = glm::inverse(vp);
    }

    rt::ray camera::genRay(uint32_t x, uint32_t y) const {
        float ndcX = (x + 0.5f) / rt::resolutionX * 2 - 1;
        float ndcY = ((rt::resolutionY - y) + 0.5f) / rt::resolutionY * 2 - 1;

        glm::vec4 nearClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 farClip = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

        glm::vec4 nearWorld = m_vpInv * nearClip;
        glm::vec4 farWorld = m_vpInv * farClip;

        nearWorld /= nearWorld.w;
        farWorld /= farWorld.w;

        return rt::ray(glm::vec3(nearWorld), glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld)));
    }

}