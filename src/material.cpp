#include "material.hpp"

#include "random.hpp"

namespace rt {
    material::material(rt::materialType type,
                       const glm::vec3& albedo, 
                       float smoothnes, 
                       const glm::vec3& emissionColur, 
                       float emissionStrength)
                        : m_type(type),
                        m_albedo(albedo),
                        m_smoothnes(smoothnes),
                        m_emissionColur(emissionColur),
                        m_emissionStrength(emissionStrength)
                        {}
    
    bool material::scatter(rt::ray& ray, const rt::hitInfo& hit, glm::vec3& attenuation, uint32_t& seed) const {
        if (m_type == LAMBERTIAN) {
            glm::vec3 newOrigin = hit.origin + hit.normal * 0.00001f;
            glm::vec3 newDirection = glm::normalize(hit.normal + randomUnitVec3(seed));
            attenuation = m_albedo;
            ray = rt::ray(newOrigin, newDirection);
            return true;
        }
    }

    glm::vec3 material::emitted() const {
        return m_emissionColur * m_emissionStrength;
    }

    rt::material createLambertian(const glm::vec3 &albedo) {
        return rt::material(rt::materialType::LAMBERTIAN, albedo, 0.0f, glm::vec3(0.0f), 0.0f);
    }
}