#include "scene.hpp"
#include "glm/geometric.hpp"
#include "hitinfo.hpp"
#include <cstddef>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tinybvh/tiny_bvh.h"

#include "tinyobj/tiny_obj_loader.h"

#include <limits>
#include <cmath>
#include <iostream>

namespace rt {
    mesh::mesh(const char* filePath) {
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

        size_t totalTriangles = 0;
        for (const auto& shape : shapes) {
            totalTriangles += shape.mesh.indices.size();
        }

        m_vertices.reserve(totalTriangles);

        for (const auto& shape : shapes) {
            for (const auto& idx : shape.mesh.indices) {
                m_vertices.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2],
                    0
                );
            }
        }

        m_bvh.Build(m_vertices.data(), (uint32_t)floor(m_vertices.size()/3.0f));
    }

    bool mesh::hit(const rt::ray& r, 
                   rt::hitInfo& resoult,  
                   const glm::mat4& inverseModelMatrix, 
                   const glm::mat3& normalMatrix) const {

        glm::vec3 localOrigin = glm::vec3(inverseModelMatrix * glm::vec4(r.origin(), 1.0f));
        glm::vec3 localDirection = glm::vec3(inverseModelMatrix * glm::vec4(r.direction(), 0.0f));

        rt::ray localRay(localOrigin, localDirection);

        tinybvh::bvhvec3 localOriginBVH(localOrigin.x, localOrigin.y, localOrigin.z);
        tinybvh::bvhvec3 localDirectionBVH(localDirection.x, localDirection.y, localDirection.z);

        tinybvh::Ray localRayBVH(localOriginBVH, localDirectionBVH);

        m_bvh.Intersect(localRayBVH);

        resoult.hasHit = false;

        if (localRayBVH.hit.t != 1e30f) {
            resoult.d = localRayBVH.hit.t / glm::length(localRay.direction());
            resoult.hasHit = true;

            uint32_t primID = localRayBVH.hit.prim;
            const tinybvh::bvhvec4& v0 = m_vertices[primID * 3 + 0];
            const tinybvh::bvhvec4& v1 = m_vertices[primID * 3 + 1];
            const tinybvh::bvhvec4& v2 = m_vertices[primID * 3 + 2];

            glm::vec3 p1(v0.x, v0.y, v0.z);
            glm::vec3 p2(v1.x, v1.y, v1.z);
            glm::vec3 p3(v2.x, v2.y, v2.z);

            resoult.normal = glm::normalize(normalMatrix * glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));
            resoult.backface = glm::dot(r.direction(), resoult.normal) > 0.0f;
            resoult.origin = r.at(resoult.d);
        }

        return resoult.hasHit;
    }

    meshInstance::meshInstance(uint32_t mesh, 
                               uint32_t material, 
                               const glm::vec3& position, 
                               const glm::vec3& rotation, 
                               const glm::vec3& scale)
                               : position(position),
                                 rotation(rotation),
                                 scale(scale),
                                 m_mesh(mesh),
                                 m_material(material) {
            
        buildMatrix();
    }

    bool meshInstance::hit(const rt::ray& r, rt::hitInfo& resoult, const std::vector<rt::mesh>& meshes) const {
        if (meshes.at(m_mesh).hit(r, resoult, m_inverseModelMatrix, m_normalMatrix)) {
            resoult.material = m_material;
            return true;
        }
        return false;
    }

    void meshInstance::buildMatrix() {
        glm::quat rot = glm::quat(glm::radians(rotation));
        glm::quat inverseRotation = glm::conjugate(rot);

        glm::vec3 inverseScale = 1.0f / scale;

        glm::vec3 inverseTranslate = -position;

        m_inverseModelMatrix = glm::mat4(1.0f);
        m_inverseModelMatrix = glm::scale(m_inverseModelMatrix, inverseScale);
        m_inverseModelMatrix = m_inverseModelMatrix * glm::mat4_cast(inverseRotation);
        m_inverseModelMatrix = glm::translate(m_inverseModelMatrix, inverseTranslate);

        m_normalMatrix = glm::transpose(glm::mat3(m_inverseModelMatrix));
    }

    bool scene::hit(const rt::ray& r, rt::hitInfo& resoult) const {

        float minDist = std::numeric_limits<float>::infinity();

        for (unsigned int i = 0; i < meshInstances.size(); i++) {
            rt::hitInfo hitTemp;

            if (meshInstances.at(i).hit(r, hitTemp, meshes) && hitTemp.d < minDist) {
                resoult = hitTemp;
                minDist = hitTemp.d;
            }
        }

        return resoult.hasHit;
    }

}