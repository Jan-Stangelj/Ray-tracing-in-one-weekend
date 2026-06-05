#pragma once

#include <glm/glm.hpp>

#include "ray.hpp"

namespace rt {

     struct hitInfo {
        glm::vec3 origin;
        glm::vec3 normal;
        float d;
     };

    class sphere {
    public:

        sphere(glm::vec3 origin, float radius, glm::vec3 albedo) : m_origin(origin), m_radius(radius), m_albedo(albedo) {}
        ~sphere() = default;

        bool hit(const rt::ray& r, rt::hitInfo resoult) {
            return true;
        }

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        float m_radius = 1.0f;

        glm::vec3 m_albedo = glm::vec3(0.0f);

    };
}