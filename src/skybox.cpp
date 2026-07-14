#include "skybox.hpp"

#include "glm/ext/scalar_constants.hpp"

#include "stb/stb_image.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace rt {
    skybox::skybox(const std::filesystem::path& filePath) {
        float* data = stbi_loadf(filePath.c_str(), &m_width, &m_height, nullptr, 3);

        if (data == nullptr) {
            std::cout << "Failed to load skybox at: " << filePath << "\n";
            return;
        }

        m_data = std::shared_ptr<float>(
            data,
            [](float* ptr) {
                stbi_image_free(static_cast<void*>(ptr));
            }
        );
    }

    glm::vec3 skybox::colorAt(glm::vec3 direction) const {

        if (m_data == nullptr){
            float skyVar = (glm::normalize(direction).y + 1) / 2;

            glm::vec3 skyColur(0.5f, 0.7f, 1.0f);
            glm::vec3 groundColur(1.0f);

            return skyVar * skyColur + (1.0f-skyVar) * groundColur;
        }

        glm::vec3 dir = glm::normalize(direction);

        float u = atan2(dir.z, dir.x) / (2.0 * glm::pi<float>()) + 0.5;
        float v = asin(dir.y) / glm::pi<float>() + 0.5f;

        v = 1.0f - v;

        int x = u * m_width;
        int y = v * m_height;

        x = std::clamp(x, 0, m_width - 1);
        y = std::clamp(y, 0, m_height - 1);

        size_t stride = (y * m_width + x) * 3;
        
        return glm::vec3(m_data.get()[stride], m_data.get()[stride+1], m_data.get()[stride+2]);
    }
}