#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

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
        void display();

        void terminate();

        rt::camera camera;
        rt::scene scene;

        GLFWwindow* window = nullptr;

    private:

        GLuint m_VBO, m_VAO, m_texture;
        rt::shader m_shader;

        std::vector<uint8_t> m_resoult = std::vector<uint8_t>(rt::resolutionX * rt::resolutionY * 3);
    };
}