#include "scene.hpp"
#include "hitinfo.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj/tiny_obj_loader.h"

#include <limits>
#include <cmath>
#include <iostream>

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

    mesh::mesh(const char* filePath, uint32_t material) : m_material(material) {
        tinyobj::ObjReaderConfig config;
        config.triangulate = true;
        config.mtl_search_path = "";

        tinyobj::ObjReader reader;
        if (!reader.ParseFromFile(filePath, config)) {
            if (!reader.Error().empty())
                std::cerr << "ERROR: " << reader.Error() << '\n';
            return;
        }
        if (!reader.Warning().empty())
            std::cout << "WARNING: " << reader.Warning() << '\n';

        const auto& attrib = reader.GetAttrib();
        const auto& shapes = reader.GetShapes();

        for (const auto& shape : shapes) {
            m_vertices.reserve(shape.mesh.indices.size());

            for (const auto& idx : shape.mesh.indices) {
                m_vertices.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
            }
        }
    }

    void mesh::buildMatrix() {
        glm::quat rot = glm::quat(glm::radians(rotation));
        glm::quat inverseRotation = glm::conjugate(rot);

        glm::vec3 inverseScale = 1.0f / scale;

        glm::vec3 inverseTranslate = -position;

        m_inverseModel = glm::mat4(1.0f);
        m_inverseModel = glm::scale(m_inverseModel, inverseScale);
        m_inverseModel = m_inverseModel * glm::mat4_cast(inverseRotation);
        m_inverseModel = glm::translate(m_inverseModel, inverseTranslate);

        m_model = glm::inverse(m_inverseModel);

        m_normalMatrix = glm::transpose(glm::mat3(m_inverseModel));
    }

    bool mesh::hit(const rt::ray& r, rt::hitInfo& resoult) const {
        float minDist = std::numeric_limits<float>::infinity();

        glm::vec3 localOrigin = glm::vec3(m_inverseModel * glm::vec4(r.origin(), 1.0f));
        glm::vec3 localDirection = glm::vec3(m_inverseModel * glm::vec4(r.direction(), 0.0f));

        rt::ray localRay = rt::ray(localOrigin, localDirection);

        for (unsigned int i = 0; i < m_vertices.size(); i += 3) {
            glm::vec3 p1 = m_vertices[i];
            glm::vec3 p2 = m_vertices[i+1];
            glm::vec3 p3 = m_vertices[i+2];

            constexpr float kEpsilon = 1e-8f;

            glm::vec3 p1p2 = p2 - p1;
            glm::vec3 p1p3 = p3 - p1;

            glm::vec3 pvec = glm::cross(localRay.direction(), p1p3);
            float det = glm::dot(p1p2, pvec);

            if (glm::abs(det) < kEpsilon)
                continue;

            float invDet = 1 / det;

            glm::vec3 tvec = localRay.origin() - p1;
            float u = glm::dot(tvec, pvec) * invDet;
            if (u < 0 || u > 1)
                continue;

            glm::vec3 qvec = glm::cross(tvec, p1p2);
            float v = glm::dot(localRay.direction(), qvec) * invDet;
            if (v < 0 || u + v > 1)
                continue;

            float d = glm::dot(p1p3, qvec) * invDet;

            if (d < kEpsilon)
                continue;

            if (d < minDist) {
                minDist = d;
                resoult.d = d;
                resoult.hasHit = true;
                resoult.material = m_material;
                resoult.normal = glm::normalize(m_normalMatrix * glm::cross(p1p2, p1p3));
                resoult.backface = glm::dot(localRay.direction(), resoult.normal) > 0.0f;
                resoult.origin = glm::vec3(m_model * glm::vec4(localRay.at(resoult.d), 1.0f));
            }

        }

        return resoult.hasHit;
    }

    bool scene::hit(const rt::ray& r, rt::hitInfo& resoult) const {

        float minDist = std::numeric_limits<float>::infinity();

        for (unsigned int i = 0; i < meshes.size(); i++) {
            rt::hitInfo hitTemp;

            if (meshes.at(i).hit(r, hitTemp) && hitTemp.d < minDist) {
                resoult = hitTemp;
                minDist = hitTemp.d;
            }
        }

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