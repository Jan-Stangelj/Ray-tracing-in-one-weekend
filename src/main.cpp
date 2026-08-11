#include "glm/ext/vector_float3.hpp"
#include "material.hpp"
#include "renderer.hpp"
#include "settings.hpp"

int main() {
    rt::renderer renderer;

    renderer.init();

    renderer.camera = rt::camera(glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(0.0f), rt::resolutionX, rt::resolutionY, rt::fovY);

    //renderer.scene.skybox = rt::skybox("../assets/skyboxes/skybox1.hdr");

    // Materials
    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.73f)));
    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.65, 0.05f, 0.05f)));
    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.05, 0.65f, 0.05f)));
    renderer.scene.materials.push_back(rt::createEmissive(glm::vec3(1.0f), 5.0f));
    renderer.scene.materials.push_back(rt::createDielectric(glm::vec3(1.0f), 0.0f, 1.5));
    renderer.scene.materials.push_back(rt::createMetal(glm::vec3(0.65, 0.05f, 0.05f), 0.0f));

    // Meshes
    renderer.scene.meshes.emplace_back("../assets/models/quad.obj");
    renderer.scene.meshes.emplace_back("../assets/models/cube.obj");
    renderer.scene.meshes.emplace_back("../assets/models/utah_teapot.obj");
    renderer.scene.meshes.emplace_back("../assets/models/dragon.obj");

    constexpr float size = 2.5f;

    // Supporting cube
    renderer.scene.meshInstances.emplace_back(
        1,
        0,
        glm::vec3(1.0f, -1.6f, 1.5f),
        glm::vec3(0.0f, -25.0f, 0.0f),
        glm::vec3(0.75f, 1.2f, 0.75f)
    );

    // Utah teapot
    renderer.scene.meshInstances.emplace_back(
        2,
        4,
        glm::vec3(1.0f, -0.4f, 1.5f),
        glm::vec3(0.0f, 165.0f, 0.0f),
        glm::vec3(0.4f)
    );

    // Dragon
    renderer.scene.meshInstances.emplace_back(
        3,
        5,
        glm::vec3(-1.2f, -3.1f, 1.5f),
        glm::vec3(0.0f, 195.0f, 0.0f),
        glm::vec3(0.12f)
    );

    //  |-----------------|
    //  |Cornell box begin|
    //  |-----------------|

    // Floor
    renderer.scene.meshInstances.emplace_back(
        0,
        0,
        glm::vec3(0.0f, -size, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Ceiling
    renderer.scene.meshInstances.emplace_back(
        0,
        0,
        glm::vec3(0.0f, size, 0.0f),
        glm::vec3(180.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Ceiling light
    renderer.scene.meshInstances.emplace_back(
        0,
        3,
        glm::vec3(0.0f, size - rt::epsilon, 0.0f),
        glm::vec3(180.0f, 0.0f, 0.0f),
        glm::vec3(size / 3.0f)
    );

    // Front wall
    renderer.scene.meshInstances.emplace_back(
        0,
        0,
        glm::vec3(0.0f, 0.0f, size),
        glm::vec3(-90.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Back wall
    renderer.scene.meshInstances.emplace_back(
        0,
        0,
        glm::vec3(0.0f, 0.0f, -size),
        glm::vec3(90.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Left wall
    renderer.scene.meshInstances.emplace_back(
        0,
        1,
        glm::vec3(size, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 90.0f),
        glm::vec3(size)
    );

    // Right wall
    renderer.scene.meshInstances.emplace_back(
        0,
        2,
        glm::vec3(-size, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -90.0f),
        glm::vec3(size)
    );

    //  |---------------|
    //  |Cornell box end|
    //  |---------------|
    
    renderer.render();

    while (!glfwWindowShouldClose(renderer.window)) {
        renderer.display();
    }

    renderer.terminate();

    return 0;
}