#include "renderer.hpp"

#include "raytracing.hpp"
#include "settings.hpp"

#include <OpenImageDenoise/oidn.hpp>
#include <chrono>
#include <iostream>

glm::vec3 PBRNeutralToneMapping( glm::vec3 color );

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam);

namespace rt {
    void renderer::init() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        // Create window and OpenGL context
        window = glfwCreateWindow(rt::resolutionX, rt::resolutionY, rt::title.cbegin(), NULL, NULL);
        if (window == NULL){
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);

        // Initialize GLAD
        if (!gladLoadGL(glfwGetProcAddress)){
            std::cerr << "Failed to initialize GLAD\n";
            return;
        }

        glfwSwapInterval(1);
        glViewport(0, 0, rt::resolutionX, rt::resolutionY);

        int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            std::clog << "OpenGL Debug Context Enabled\n";
        }

        m_shader = rt::shader("../assets/shaders/basic.vert", "../assets/shaders/basic.frag");

        float vertices[] = {
            1.0f, 1.0f,
            -1.0f, -1.0f,
            -1.0f, 1.0f,

            1.0f, 1.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenTextures(1, &m_texture);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Prepare denoiser
        m_device = oidn::newDevice(oidn::DeviceType::CPU);
        m_device.commit();

        m_beautyFilter = m_device.newFilter("RT");
        m_beautyFilter.setImage("color", m_beauty.data(), oidn::Format::Float3, rt::resolutionX, rt::resolutionY);
        m_beautyFilter.setImage("albedo", m_albedo.data(), oidn::Format::Float3, rt::resolutionX, rt::resolutionY);
        m_beautyFilter.setImage("normal", m_normal.data(), oidn::Format::Float3, rt::resolutionX, rt::resolutionY);
        m_beautyFilter.setImage("output", m_beauty.data(), oidn::Format::Float3, rt::resolutionX, rt::resolutionY);
        m_beautyFilter.set("hdr", true);
        m_beautyFilter.set("quality", OIDN_QUALITY_HIGH);
        m_beautyFilter.set("cleanAux", rt::prefilterAlbedo); // The albedo texture has some noise, if not prefiltered
        m_beautyFilter.commit();

        if (rt::prefilterAlbedo) {
            m_albedoFilter = m_device.newFilter("RT");
            m_albedoFilter.setImage("albedo", m_albedo.data(), oidn::Format::Float3, rt::resolutionX, rt::resolutionY);
            m_albedoFilter.setImage("output", m_albedo.data(), oidn::Format::Float3, rt::resolutionX, rt::resolutionY);
            m_albedoFilter.set("hdr", false);
            m_albedoFilter.set("quality", OIDN_QUALITY_HIGH);
            m_albedoFilter.commit();
        }
    }

    void renderer::denoise() {

        if (rt::prefilterAlbedo) 
            m_albedoFilter.execute();

        m_beautyFilter.execute();

        const char* errorMessage;
        if (m_device.getError(errorMessage) != oidn::Error::None)
            std::cout << "OIDN ERROR: " << errorMessage << '\n';
    }

    void renderer::postProcess() {
        #pragma omp parallel for
        for (unsigned int y = 0; y < rt::resolutionY; y++) {

            for (unsigned int x = 0; x < rt::resolutionX; x++) {

                uint32_t index = (y * rt::resolutionX + x) * 3;

                std::vector<float>& input = m_beauty;

                glm::vec3 color(
                    input.at(index),
                    input.at(index + 1),
                    input.at(index + 2)
                );

                color = PBRNeutralToneMapping(color);

                color = glm::pow(color, glm::vec3(1.0f / 2.2f));

                m_resoult.at(index)     = static_cast<uint8_t>(color.r * 255.0f);
                m_resoult.at(index + 1) = static_cast<uint8_t>(color.g * 255.0f);
                m_resoult.at(index + 2) = static_cast<uint8_t>(color.b * 255.0f);
            }
        }
    }

    void renderer::render() {
        auto start = std::chrono::high_resolution_clock::now();

        rt::render(m_beauty, m_albedo, m_normal, camera, scene);
        if (rt::denoise) denoise();
        postProcess();
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rt::resolutionX, rt::resolutionY, 0, GL_RGB, GL_UNSIGNED_BYTE, m_resoult.data());

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::ratio<1, 1>> ms = end - start;
        
        std::cout << "Rendering took: " << ms.count() << " s\n";
    }

    void renderer::display() {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, m_texture);

        m_shader.use();
        m_shader.setInt("texture1", 0);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void renderer::terminate() {
        glfwTerminate();
    }
}

glm::vec3 PBRNeutralToneMapping( glm::vec3 color ) {
  const float startCompression = 0.8 - 0.04;
  const float desaturation = 0.15;

  float x = std::min(color.r, std::min(color.g, color.b));
  float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
  color -= offset;

  float peak = std::max(color.r, std::max(color.g, color.b));
  if (peak < startCompression) return color;

  const float d = 1. - startCompression;
  float newPeak = 1. - d * d / (peak + d - startCompression);
  color *= newPeak / peak;

  float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
  return mix(color, newPeak * glm::vec3(1, 1, 1), g);
}

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cerr << "---------------" << std::endl;
    std::cerr << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
    } std::cerr << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
    } std::cerr << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;
    } std::cerr << std::endl;
    std::cerr << std::endl;
    glfwTerminate();
}