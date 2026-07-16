#include "scene.hpp"

#include <limits>
#include <cmath>

namespace rt {

    sphere::sphere(glm::vec3 origin, float radius, uint32_t material) : m_origin(origin), 
                                                                        m_radius(radius), 
                                                                        m_material(material) {}

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
            result.material = m_material;
            return true;
        }

        // Far intersection
        t = (h + sqrtD) / a;
        if (t > 0.001f) {
            result.d = t;
            result.origin = r.at(t);
            result.normal = glm::normalize(result.origin - m_origin);
            result.hasHit = true;
            result.material = m_material;
            return true;
        }

        return false;
    }

    uint32_t sphere::getMaterial() const {
        return m_material;
    }


    bool scene::hit(const rt::ray& r, rt::hitInfo& resoult) const {

        float minDist = std::numeric_limits<float>::infinity();

        for (unsigned int i = 0; i < spheres.size(); i++) {
            rt::hitInfo hitTemp;

            if (spheres.at(i).hit(r, hitTemp) && hitTemp.d < minDist){
                resoult = hitTemp;
                minDist = hitTemp.d;
            }
        }

        return resoult.hasHit;
    }

}