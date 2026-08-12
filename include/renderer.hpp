#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <OpenImageDenoise/oidn.hpp>

#include "camera.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "shader.hpp"

namespace rt {
    class renderer {
    public:

        renderer() = default;
        ~renderer() = default;

        void init();

        void render();
        void displayUI();
        void display();

        void terminate();

        rt::camera camera;
        rt::scene scene;

        GLFWwindow* window = nullptr;

    private:
    
        void denoise();
        void postProcess();

        std::chrono::duration<double, std::ratio<1, 1>> m_renderingTime;

        GLuint m_VBO, m_VAO, m_texture;
        rt::shader m_shader;

        oidn::DeviceRef m_device;
        oidn::FilterRef m_albedoFilter;
        oidn::FilterRef m_beautyFilter;

        std::vector<float> m_beauty = std::vector<float>(rt::resolutionX * rt::resolutionY * 3);
        std::vector<float> m_albedo = std::vector<float>(rt::resolutionX * rt::resolutionY * 3);
        std::vector<float> m_normal = std::vector<float>(rt::resolutionX * rt::resolutionY * 3);
        std::vector<uint8_t> m_resoult = std::vector<uint8_t>(rt::resolutionX * rt::resolutionY * 3);
    };
}