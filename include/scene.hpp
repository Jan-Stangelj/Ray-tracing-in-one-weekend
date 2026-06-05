#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <type_traits>
#include <typeinfo>
#include <vector>

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

bool hit(const rt::ray& r, rt::hitInfo& result) const {
            glm::vec3 oc = m_origin - r.origin();
            float a = glm::dot(r.direction(), r.direction());
            float h = glm::dot(r.direction(), oc);
            float c = glm::dot(oc, oc) - m_radius * m_radius;

            float discriminant = h * h - a * c;
            if (discriminant < 0.0f) return false;

            float sqrtD = std::sqrt(discriminant);

            // Closest intersection (smaller t)
            float t = (h - sqrtD) / a;
            if (t > 0.001f) {
                result.d = t;
                result.origin = r.at(t);
                result.normal = glm::normalize(result.origin - m_origin);
                return true;
            }

            // Far intersection
            t = (h + sqrtD) / a;
            if (t > 0.001f) {
                result.d = t;
                result.origin = r.at(t);
                result.normal = glm::normalize(result.origin - m_origin);
                return true;
            }

            return false;
        }

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        float m_radius = 1.0f;

        glm::vec3 m_albedo = glm::vec3(0.0f);

    };

    class scene {
    public:

        scene() = default;
        ~scene() = default;

        std::vector<rt::sphere> spheres = {};

        bool hit(const rt::ray& r, rt::hitInfo& resoult) const {
            float minDist = std::numeric_limits<float>::infinity();
            bool hasHit = false;

            for (const rt::sphere& s : spheres) {
                rt::hitInfo hitTemp;

                if (s.hit(r, hitTemp) && hitTemp.d < minDist){
                    resoult = hitTemp;
                    minDist = hitTemp.d;
                    hasHit = true;
                }
            }

            return hasHit;
        }
    };
}