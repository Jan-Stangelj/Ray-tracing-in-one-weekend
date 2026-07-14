#include "renderer.hpp"

int main() {
    rt::renderer renderer;

    renderer.init();

    renderer.camera = rt::camera(glm::vec3(0.0f, 1.0f, -3.0f), glm::vec3(0.0f), rt::resolutionX, rt::resolutionY, rt::fovY);

    renderer.scene.spheres.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.5f, 0.3f, 0.3f), glm::vec3(0.0f));
    renderer.scene.spheres.emplace_back(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, glm::vec3(0.3f, 0.5f, 0.3f), glm::vec3(0.0f));
    renderer.scene.spheres.emplace_back(glm::vec3(-65.0f, 5.0f, 100.0f), 30.0f, glm::vec3(0.0f), glm::vec3(100.0f, 20.0f, 20.0f));

    renderer.render();

    while (!glfwWindowShouldClose(renderer.window)) {
        renderer.display();
    }

    renderer.terminate();

    return 0;
}