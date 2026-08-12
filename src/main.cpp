#include "glm/ext/vector_float3.hpp"
#include "material.hpp"
#include "renderer.hpp"
#include "settings.hpp"

int main() {
    rt::renderer renderer;

    renderer.init();

    renderer.camera = rt::camera(
        glm::vec3(0.0f, 0.0f, -2.5f),
        glm::vec3(0.0f),
        rt::resolutionX,
        rt::resolutionY,
        rt::fovY
    );

    // renderer.scene.skybox = rt::skybox("../assets/skyboxes/skybox1.hdr");

    // -------------------------------------------------------------------------
    // Materials
    // -------------------------------------------------------------------------

    renderer.scene.materials["white"] =
        rt::createLambertian(glm::vec3(0.73f));

    renderer.scene.materials["red"] =
        rt::createLambertian(glm::vec3(0.65f, 0.05f, 0.05f));

    renderer.scene.materials["green"] =
        rt::createLambertian(glm::vec3(0.05f, 0.65f, 0.05f));

    renderer.scene.materials["light"] =
        rt::createEmissive(glm::vec3(1.0f), 5.0f);

    renderer.scene.materials["glass"] =
        rt::createDielectric(glm::vec3(1.0f), 0.0f, 1.5f);

    renderer.scene.materials["clearcoat"] =
        rt::createClearCoat(
            glm::vec3(0.65f, 0.05f, 0.05f),
            glm::vec3(1.0f),
            0.0f,
            1.4f
        );

    renderer.scene.materials["metal"] =
        rt::createMetal(
            glm::vec3(0.65f, 0.05f, 0.05f),
            0.0f
        );

    // -------------------------------------------------------------------------
    // Meshes
    // -------------------------------------------------------------------------

    renderer.scene.meshes.try_emplace(
        "quad",
        "../assets/models/quad.obj"
    );

    renderer.scene.meshes.try_emplace(
        "cube",
        "../assets/models/cube.obj"
    );

    renderer.scene.meshes.try_emplace(
        "teapot",
        "../assets/models/utah_teapot.obj"
    );

    renderer.scene.meshes.try_emplace(
        "dragon",
        "../assets/models/dragon.obj"
    );

    constexpr float size = 2.5f;

    // -------------------------------------------------------------------------
    // Objects
    // -------------------------------------------------------------------------

    // Supporting cube
    renderer.scene.meshInstances["supporting_cube"] = rt::meshInstance(
        "cube",
        "white",
        glm::vec3(1.0f, -1.6f, 1.5f),
        glm::vec3(0.0f, -25.0f, 0.0f),
        glm::vec3(0.75f, 1.2f, 0.75f)
    );

    // Utah teapot
    renderer.scene.meshInstances["teapot"] = rt::meshInstance(
        "teapot",
        "glass",
        glm::vec3(1.0f, -0.4f, 1.5f),
        glm::vec3(0.0f, 165.0f, 0.0f),
        glm::vec3(0.4f)
    );

    // Dragon
    renderer.scene.meshInstances["dragon"] = rt::meshInstance(
        "dragon",
        "clearcoat",
        glm::vec3(-1.2f, -3.1f, 1.5f),
        glm::vec3(0.0f, 195.0f, 0.0f),
        glm::vec3(0.12f)
    );

    // -------------------------------------------------------------------------
    // Cornell box
    // -------------------------------------------------------------------------

    // Floor
    renderer.scene.meshInstances["floor"] = rt::meshInstance(
        "quad",
        "white",
        glm::vec3(0.0f, -size, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Ceiling
    renderer.scene.meshInstances["ceiling"] = rt::meshInstance(
        "quad",
        "white",
        glm::vec3(0.0f, size, 0.0f),
        glm::vec3(180.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Ceiling light
    renderer.scene.meshInstances["ceiling_light"] = rt::meshInstance(
        "quad",
        "light",
        glm::vec3(0.0f, size - rt::epsilon, 0.0f),
        glm::vec3(180.0f, 0.0f, 0.0f),
        glm::vec3(size / 2.0f)
    );

    // Front wall
    renderer.scene.meshInstances["front_wall"] = rt::meshInstance(
        "quad",
        "white",
        glm::vec3(0.0f, 0.0f, size),
        glm::vec3(-90.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Back wall
    renderer.scene.meshInstances["back_wall"] = rt::meshInstance(
        "quad",
        "white",
        glm::vec3(0.0f, 0.0f, -size),
        glm::vec3(90.0f, 0.0f, 0.0f),
        glm::vec3(size)
    );

    // Left wall
    renderer.scene.meshInstances["left_wall"] = rt::meshInstance(
        "quad",
        "red",
        glm::vec3(size, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 90.0f),
        glm::vec3(size)
    );

    // Right wall
    renderer.scene.meshInstances["right_wall"] = rt::meshInstance(
        "quad",
        "green",
        glm::vec3(-size, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -90.0f),
        glm::vec3(size)
    );

    // -------------------------------------------------------------------------
    // Render
    // -------------------------------------------------------------------------

    renderer.render();

    while (!glfwWindowShouldClose(renderer.window)) {
        renderer.display();
    }

    renderer.terminate();

    return 0;
}