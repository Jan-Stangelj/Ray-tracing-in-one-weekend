#include "material.hpp"
#include "renderer.hpp"

int main() {
    rt::renderer renderer;

    renderer.init();

    renderer.camera = rt::camera(glm::vec3(0.0f, 0.5f, -3.0f), glm::vec3(0.0f), rt::resolutionX, rt::resolutionY, rt::fovY);

    renderer.scene.skybox = rt::skybox("../assets/skyboxes/skybox1.hdr");

    renderer.scene.materials.push_back(rt::createDielectric(glm::vec3(1.0f), 0.0f, 1.5));
    renderer.scene.materials.push_back(rt::createEmissive(glm::vec3(1.0f), 5.0f));
    renderer.scene.materials.push_back(rt::createMetal(glm::vec3(0.2f, 0.2f, 0.6f), 0.15f));
    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.2f)));

    renderer.scene.spheres.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, 0);
    renderer.scene.spheres.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, 1);
    renderer.scene.spheres.emplace_back(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f, 2);

    renderer.scene.spheres.emplace_back(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, 3);

    renderer.render();

    while (!glfwWindowShouldClose(renderer.window)) {
        renderer.display();
    }

    renderer.terminate();

    return 0;
}