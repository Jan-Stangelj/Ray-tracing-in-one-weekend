#pragma once

#include <filesystem>

#include <glm/glm.hpp>
#include <memory>

namespace rt {
    class skybox {
    public:

        skybox(const std::filesystem::path& filePath);
        skybox() = default;
        ~skybox() = default;

        glm::vec3 colorAt(glm::vec3 direction) const;

    private:

        int32_t m_width = 0;
        int32_t m_height = 0;

        std::shared_ptr<float> m_data = nullptr;

    };
}