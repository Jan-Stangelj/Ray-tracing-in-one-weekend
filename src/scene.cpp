#include "scene.hpp"

#include <limits>
#include <cmath>

namespace rt {

    sphere::sphere(glm::vec3 origin, float radius, glm::vec3 albedo, float smoothnes, glm::vec3 emission) : m_origin(origin), 
                                                                                             m_radius(radius), 
                                                                                             m_albedo(albedo),
                                                                                             m_smoothnes(smoothnes),
                                                                                             m_emission(emission) {}

    bool sphere::hit(const rt::ray& r, rt::hitInfo& result) const {

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
            result.hasHit = true;
            return true;
        }

        // Far intersection
        t = (h + sqrtD) / a;
        if (t > 0.001f) {
            result.d = t;
            result.origin = r.at(t);
            result.normal = glm::normalize(result.origin - m_origin);
            result.hasHit = true;
            return true;
        }

        return false;
    }

    glm::vec3 sphere::getColur() const {
        return m_albedo;
    }

    float sphere::getSmoothnes() const {
        return m_smoothnes;
    }

    glm::vec3 sphere::getEmission() const {
        return m_emission;
    }


    bool scene::hit(const rt::ray& r, rt::hitInfo& resoult) const {

        float minDist = std::numeric_limits<float>::infinity();

        for (unsigned int i = 0; i < spheres.size(); i++) {
            rt::hitInfo hitTemp;

            if (spheres.at(i).hit(r, hitTemp) && hitTemp.d < minDist){
                resoult = hitTemp;
                minDist = hitTemp.d;
                resoult.sphere = i;
            }
        }

        return resoult.hasHit;
    }

}