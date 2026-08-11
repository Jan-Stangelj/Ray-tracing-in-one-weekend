#include "material.hpp"

#include "glm/common.hpp"
#include "glm/geometric.hpp"
#include "random.hpp"
#include "settings.hpp"
#include <optional>

namespace rt {
    material::material(rt::materialType type,
                       const glm::vec3& albedo,
                       const glm::vec3& coatAlbedo,
                       float roughness, 
                       const glm::vec3& emissionColur, 
                       float emissionStrength,
                       float IOR)
                        : m_type(type),
                        m_albedo(albedo),
                        m_coatAlbedo(coatAlbedo),
                        m_roughness(roughness),
                        m_emissionColur(emissionColur),
                        m_emissionStrength(emissionStrength),
                        m_IOR(IOR)
                        {}
    
    bool material::scatter(rt::ray& ray, const rt::hitInfo& hit, glm::vec3& attenuation, uint32_t& seed) const {
        if (m_type == LAMBERTIAN) {
            glm::vec3 newOrigin = hit.origin + hit.normal.value() * rt::epsilon;
            glm::vec3 newDirection = glm::normalize(hit.normal.value() + randomUnitVec3(seed));
            attenuation = m_albedo;
            ray = rt::ray(newOrigin, newDirection);
            return true;
        }
        else if (m_type == METAL) {
            glm::vec3 newOrigin = hit.origin + hit.normal.value() * rt::epsilon;
            glm::vec3 newDirection = glm::normalize(glm::reflect(ray.direction(), hit.normal.value()) + randomUnitVec3(seed) * m_roughness);
            attenuation = m_albedo;
            ray = rt::ray(newOrigin, newDirection);
            return true;
        }
        else if (m_type == EMISSIVE) {
            return false;
        }
        else if (m_type == DIELECTRIC) {
            attenuation = m_albedo;

            glm::vec3 normal = hit.backface ? -hit.normal.value() : hit.normal.value();
            float IOR = hit.backface ? (m_IOR / rt::airIOR) : (rt::airIOR / m_IOR);

            glm::vec3 newOrigin = hit.origin - normal * rt::epsilon;
            glm::vec3 newDirection = glm::refract(ray.direction(), normal, IOR);
            float totalInterReflection = glm::length(newDirection);
            newDirection += randomUnitVec3(seed) * m_roughness;

            float cosTheta = glm::min(dot(-ray.direction(), normal), 1.0f);
            if ((totalInterReflection == 0.0f) || (fresnelSchlick(cosTheta, m_IOR) > randomFloat(seed))) {
                newDirection = glm::normalize(glm::reflect(ray.direction(), normal) + randomUnitVec3(seed) * m_roughness);
                newOrigin += normal * rt::epsilon * 2.0f;
            }

            ray = rt::ray(newOrigin, newDirection);

            return true;
        }
        else if (m_type == CLEARCOAT) {
            attenuation = m_albedo;

            glm::vec3 normal = hit.backface ? -hit.normal.value() : hit.normal.value();
            float IOR = hit.backface ? (m_IOR / rt::airIOR) : (rt::airIOR / m_IOR);

            glm::vec3 newOrigin = hit.origin + normal * rt::epsilon;
            glm::vec3 newDirection = normal + randomUnitVec3(seed);
            float totalInterReflection = glm::length(newDirection);
            newDirection += randomUnitVec3(seed) * m_roughness;

            float cosTheta = glm::min(dot(-ray.direction(), normal), 1.0f);
            if ((totalInterReflection == 0.0f) || (fresnelSchlick(cosTheta, m_IOR) > randomFloat(seed))) {
                newDirection = glm::normalize(glm::reflect(ray.direction(), normal) + randomUnitVec3(seed) * m_roughness);
                attenuation = m_coatAlbedo;
            }

            ray = rt::ray(newOrigin, newDirection);

            return true;
        }
        return false;
    }

    glm::vec3 material::emitted() const {
        return m_emissionColur * m_emissionStrength;
    }

    std::optional<glm::vec3> material::albedo() const {
        if (m_type == rt::materialType::DIELECTRIC) return std::nullopt;
        return m_albedo;
    }

    rt::material createLambertian(const glm::vec3 &albedo) {
        return rt::material(rt::materialType::LAMBERTIAN, albedo, glm::vec3(0.0f), 0.0f, glm::vec3(0.0f), 0.0f, 1.0f);
    }
    rt::material createMetal(const glm::vec3 &albedo, float roughness) {
        return rt::material(rt::materialType::METAL, albedo, glm::vec3(0.0f), roughness, glm::vec3(0.0f), 0.0f, 1.0f);
    }
    rt::material createEmissive(const glm::vec3 &emissionColur, float emissionStrength) {
        return rt::material(rt::materialType::EMISSIVE, glm::vec3(0.0f), glm::vec3(0.0f), 0.0f, emissionColur, emissionStrength, 1.0f);
    }
    rt::material createDielectric(const glm::vec3 &albedo, float roughness, float IOR) {
        return rt::material(rt::materialType::DIELECTRIC, albedo, glm::vec3(0.0f), roughness, glm::vec3(0.0f), 0.0f, IOR);
    }
    rt::material createClearCoat(const glm::vec3& albedo, const glm::vec3& coatAlbedo, float roughness, float IOR) {
        return rt::material(rt::materialType::CLEARCOAT, albedo, coatAlbedo, roughness, glm::vec3(0.0f), 0.0f, IOR);
    }
    

    float material::fresnelSchlick(float cosTheta, float IOR) {
        float r0 = (1 - IOR) / (1 + IOR);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosTheta),5);
    }
}