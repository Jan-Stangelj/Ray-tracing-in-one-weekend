#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "ray.hpp"
#include "skybox.hpp"
#include "hitinfo.hpp"
#include "material.hpp"

namespace rt {
    class sphere {
    public:

        sphere() = default;
        sphere(glm::vec3 origin, float radius, uint32_t material);
        ~sphere() = default;

        bool hit(const rt::ray& r, rt::hitInfo& result) const;

        uint32_t getMaterial() const;

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        float m_radius = 1.0f;

        uint32_t m_material;

    };

    class scene {
    public:

        scene() = default;
        ~scene() = default;

        std::vector<rt::sphere> spheres = {};
        std::vector<rt::material> materials = {};

        rt::skybox skybox;

        bool hit(const rt::ray& r, rt::hitInfo& resoult) const;

    };

}