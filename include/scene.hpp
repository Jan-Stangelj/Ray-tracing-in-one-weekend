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

    private:

        glm::vec3 m_origin = glm::vec3(0.0f);
        float m_radius = 1.0f;

        uint32_t m_material = UINT32_MAX;

    };

    class mesh {
    public:

        mesh() = default;
        mesh(const char* filePath, uint32_t material);
        ~mesh() = default;

        void buildMatrix();

        bool hit(const rt::ray& r, rt::hitInfo& resoult) const;

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

    private:

        std::vector<glm::vec3> m_vertices;

        glm::mat4 m_model = glm::mat4(1.0f);
        glm::mat4 m_inverseModel = glm::mat4(1.0f);
        glm::mat3 m_normalMatrix = glm::mat3(1.0f);

        uint32_t m_material = UINT32_MAX;

    };

    class scene {
    public:

        scene() = default;
        ~scene() = default;

        std::vector<rt::mesh> meshes = {};
        std::vector<rt::sphere> spheres = {};

        std::vector<rt::material> materials = {};

        rt::skybox skybox;

        bool hit(const rt::ray& r, rt::hitInfo& resoult) const;

    };

}