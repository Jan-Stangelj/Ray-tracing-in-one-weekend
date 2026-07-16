#pragma once

#include "ray.hpp"
#include "hitinfo.hpp"

#include <glm/glm.hpp>

namespace rt {
    enum materialType {
        LAMBERTIAN,
        METAL,
        LIGHT
    };

    class material {
    public:

        material(rt::materialType type, const glm::vec3& albedo, float smoothnes, const glm::vec3& emissionColur, float emissionStrength);
        ~material() = default;

        bool scatter(rt::ray& ray, const rt::hitInfo& hit, glm::vec3& attenuation, uint32_t& seed) const;
        glm::vec3 emitted() const;

    private:

        rt::materialType m_type = rt::materialType::LAMBERTIAN;

        glm::vec3 m_albedo = glm::vec3(0.0f);
        float m_smoothnes = 0.0f;

        glm::vec3 m_emissionColur = glm::vec3(0.0f);
        float m_emissionStrength = 0.0f;

    };

    rt::material createLambertian(const glm::vec3& albedo);
    rt::material createMetal(const glm::vec3& albedo, float smoothnes);
    rt::material createEmissive(const glm::vec3& emissionColur, float emissionStrength);
}