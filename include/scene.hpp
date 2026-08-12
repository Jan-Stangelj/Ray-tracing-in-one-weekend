#pragma once

#include "ray.hpp"
#include "skybox.hpp"
#include "hitinfo.hpp"
#include "material.hpp"

#include <glm/glm.hpp>

#include "tinybvh/tiny_bvh.h"

#include <cstdint>
#include <vector>

namespace rt {
    class mesh {
    public:

        mesh() = default;
        mesh(const char* filePath);

        // Cannot be copied
        mesh(const mesh&) = delete;
        mesh& operator=(const mesh&) = delete;

        // Explicitly movable
        mesh(mesh&&) noexcept = default;
        mesh& operator=(mesh&&) noexcept = default;

        ~mesh() = default;

        bool hit(const rt::ray& r, 
                 rt::hitInfo& resoult,
                 const glm::mat4& inverseModelMatrix, 
                 const glm::mat3& normalMatrix) const;

    private:

        std::vector<tinybvh::bvhvec4> m_vertices;
        tinybvh::BVH m_bvh;

    };

    class meshInstance {
    public:

        meshInstance() = default;
        meshInstance(uint32_t mesh, 
                     uint32_t material, 
                     const glm::vec3& position = glm::vec3(0.0f), 
                     const glm::vec3& rotation = glm::vec3(0.0f), 
                     const glm::vec3& scale = glm::vec3(1.0f));

        ~meshInstance() = default;

        bool hit(const rt::ray& r, 
                 rt::hitInfo& resoult, 
                 const std::vector<rt::mesh>& meshes) const;

        void buildMatrix();

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        uint32_t m_mesh = UINT32_MAX;
        uint32_t m_material = UINT32_MAX;

    private:

        glm::mat4 m_inverseModelMatrix = glm::mat4(1.0f);
        glm::mat3 m_normalMatrix = glm::mat3(1.0f);

    };

    class scene {
    public:

        scene() = default;
        ~scene() = default;

        std::vector<rt::meshInstance> meshInstances = {};

        std::vector<rt::mesh> meshes = {};
        std::vector<rt::material> materials = {};

        rt::skybox skybox;

        bool hit(const rt::ray& r, rt::hitInfo& resoult) const;

    };

}