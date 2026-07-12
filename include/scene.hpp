#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "ray.hpp"

namespace rt {

    struct hitInfo {
        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        float d = 0.0f;
        uint32_t sphere = UINT32_MAX;

        glm::vec3 solidAdd = glm::vec3(0.0f);
    };

    class sphere {
    public:

        sphere() = default;
        sphere(glm::vec3 origin, float radius, glm::vec3 albedo, glm::vec3 emission);
        ~sphere() = default;

        bool hit(const rt::ray& r, rt::hitInfo& result) const;

        glm::vec3 getColur() const;
        glm::vec3 getEmission() const;

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        float m_radius = 1.0f;

        glm::vec3 m_albedo = glm::vec3(0.0f);
        glm::vec3 m_emission = glm::vec3(0.0f);

    };

    class scene {
    public:

        scene() = default;
        ~scene() = default;

        std::vector<rt::sphere> spheres = {};

        bool hit(const rt::ray& r, rt::hitInfo& resoult) const;

    };

}