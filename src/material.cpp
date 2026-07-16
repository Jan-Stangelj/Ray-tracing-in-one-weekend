#include "material.hpp"

#include "glm/geometric.hpp"
#include "random.hpp"

namespace rt {
    material::material(rt::materialType type,
                       const glm::vec3& albedo, 
                       float roughness, 
                       const glm::vec3& emissionColur, 
                       float emissionStrength)
                        : m_type(type),
                        m_albedo(albedo),
                        m_roughness(roughness),
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
        else if (m_type == METAL) {
            glm::vec3 newOrigin = hit.origin + hit.normal * 0.00001f;
            glm::vec3 newDirection = glm::normalize(glm::reflect(ray.direction(), hit.normal) + randomUnitVec3(seed) * m_roughness);
            attenuation = m_albedo;
            ray = rt::ray(newOrigin, newDirection);
            return true;
        }
        else if (m_type == EMISSIVE) {
            return false;
        }
        return false;
    }

    glm::vec3 material::emitted() const {
        return m_emissionColur * m_emissionStrength;
    }

    rt::material createLambertian(const glm::vec3 &albedo) {
        return rt::material(rt::materialType::LAMBERTIAN, albedo, 0.0f, glm::vec3(0.0f), 0.0f);
    }
    rt::material createMetal(const glm::vec3 &albedo, float roughness) {
        return rt::material(rt::materialType::METAL, albedo, roughness, glm::vec3(0.0f), 0.0f);
    }
    rt::material createEmissive(const glm::vec3 &emissionColur, float emissionStrength) {
        return rt::material(rt::materialType::EMISSIVE, glm::vec3(0.0f), 0.0f, emissionColur, emissionStrength);
    }
}