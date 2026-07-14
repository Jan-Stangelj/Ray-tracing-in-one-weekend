#include "renderer.hpp"
#include "skybox.hpp"

int main() {
    rt::renderer renderer;

    renderer.init();

    renderer.camera = rt::camera(glm::vec3(0.0f, 0.5f, -3.0f), glm::vec3(0.0f), rt::resolutionX, rt::resolutionY, rt::fovY);

    renderer.scene.skybox = rt::skybox("../assets/skyboxes/skybox1.hdr");

    renderer.scene.spheres.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.6f, 0.2f, 0.2f), 0.0f, glm::vec3(0.0f));
    renderer.scene.spheres.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.0f), 0.0f, glm::vec3(5.0f));
    renderer.scene.spheres.emplace_back(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.2f, 0.2f, 0.6f), 1.0f, glm::vec3(0.0f));

    renderer.scene.spheres.emplace_back(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, glm::vec3(0.3f, 0.5f, 0.3f), 0.0f, glm::vec3(0.0f));

    renderer.render();

    while (!glfwWindowShouldClose(renderer.window)) {
        renderer.display();
    }

    renderer.terminate();

    return 0;
}