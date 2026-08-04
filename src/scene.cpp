#include "scene.hpp"
#include "glm/geometric.hpp"

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
            result.backface = glm::dot(r.direction(), result.normal) > 0.0f;
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
            result.backface = glm::dot(r.direction(), result.normal) > 0.0f;
            result.material = m_material;
            return true;
        }

        return false;
    }

    uint32_t sphere::getMaterial() const {
        return m_material;
    }


    triangle::triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, uint32_t material) : m_p1(p1),
                                                                                                            m_p2(p2),
                                                                                                            m_p3(p3),
                                                                                                            m_material(material) {}
    
    bool triangle::hit(const rt::ray& r, rt::hitInfo& resoult) const {
        constexpr float kEpsilon = 1e-8f;

        glm::vec3 p1p2 = m_p2 - m_p1;
        glm::vec3 p1p3 = m_p3 - m_p1;

        glm::vec3 pvec = glm::cross(r.direction(), p1p3);
        float det = glm::dot(p1p2, pvec);

        if (glm::abs(det) < kEpsilon)
            return false;

        float invDet = 1 / det;

        glm::vec3 tvec = r.origin() - m_p1;
        float u = glm::dot(tvec, pvec) * invDet;
        if (u < 0 || u > 1)
            return false;

        glm::vec3 qvec = glm::cross(tvec, p1p2);
        float v = glm::dot(r.direction(), qvec) * invDet;
        if (v < 0 || u + v > 1)
            return false;

        float d = glm::dot(p1p3, qvec) * invDet;

        if (d < kEpsilon)
            return false;

        resoult.d = d;
        resoult.hasHit = true;
        resoult.material = m_material;
        resoult.normal = glm::normalize(glm::cross(p1p2, p1p3));
        resoult.backface = glm::dot(r.direction(), resoult.normal) > 0.0f;
        resoult.origin = r.at(resoult.d);

        return true;
    }
    uint32_t triangle::getMaterial() const {
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

        for (unsigned int i = 0; i < triangles.size(); i++) {
            rt::hitInfo hitTemp;

            if (triangles.at(i).hit(r, hitTemp) && hitTemp.d < minDist){
                resoult = hitTemp;
                minDist = hitTemp.d;
            }
        }

        return resoult.hasHit;
    }

}