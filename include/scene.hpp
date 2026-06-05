#pragma once

#include <glm/glm.hpp>

#include "ray.hpp"

namespace rt {

     struct hitInfo {
        glm::vec3 origin = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        float d = 0.0f;
     };

    class sphere {
    public:

        sphere(glm::vec3 origin, float radius, glm::vec3 albedo) : m_origin(origin), m_radius(radius), m_albedo(albedo) {}
        ~sphere() = default;

        bool hit(const rt::ray& r, rt::hitInfo resoult) {
            glm::vec3 oc = m_origin - r.origin();
            float a = glm::pow(glm::length(r.direction()), 2);
            float h = dot(r.direction(), oc);
            float c = glm::pow(glm::length(oc), 2) - m_radius*m_radius;
            float discriminant = h*h - a*c;

            if (discriminant < 0) {
                return false;
            } else {
                resoult.d = (h - std::sqrt(discriminant)) / a;
                resoult.origin = r.at(resoult.d);
                resoult.normal = glm::normalize(resoult.origin - m_origin);
                return true;
            }
        }

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        float m_radius = 1.0f;

        glm::vec3 m_albedo = glm::vec3(0.0f);

    };
}