#include "material.hpp"
#include "renderer.hpp"
#include "settings.hpp"

int main() {
    rt::renderer renderer;

    renderer.init();

    renderer.camera = rt::camera(glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(0.0f), rt::resolutionX, rt::resolutionY, rt::fovY);

    //renderer.scene.skybox = rt::skybox("../assets/skyboxes/skybox1.hdr");

    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.73f)));
    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.65, 0.05f, 0.05f)));
    renderer.scene.materials.push_back(rt::createLambertian(glm::vec3(0.05, 0.65f, 0.05f)));
    renderer.scene.materials.push_back(rt::createEmissive(glm::vec3(1.0f), 5.0f));
    renderer.scene.materials.push_back(rt::createDielectric(glm::vec3(1.0f), 0.0f, 1.5));

    {
        rt::mesh mesh("../assets/models/cube.obj", 4);
        mesh.scale = glm::vec3(1.0f);
        mesh.rotation = glm::vec3(0.0f, 45.0f, 0.0f);
        mesh.position = glm::vec3(0.0f, -1.5f + rt::epsilon, 1.0f);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    constexpr float size = 2.5f;

    // floor
    {
        rt::mesh mesh("../assets/models/quad.obj", 0);
        mesh.scale = glm::vec3(size);
        mesh.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        mesh.position = glm::vec3(0.0f, -size, 0.0f);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    // ceiling
    {
        rt::mesh mesh("../assets/models/quad.obj", 0);
        mesh.scale = glm::vec3(size);
        mesh.rotation = glm::vec3(180.0f, 0.0f, 0.0f);
        mesh.position = glm::vec3(0.0f, size, 0.0f);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    // ceiling light
    {
        rt::mesh mesh("../assets/models/quad.obj", 3);
        mesh.scale = glm::vec3(size / 3);
        mesh.rotation = glm::vec3(180.0f, 0.0f, 0.0f);
        mesh.position = glm::vec3(0.0f, size - rt::epsilon, 0.0f);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    // front wall
    {
        rt::mesh mesh("../assets/models/quad.obj", 0);
        mesh.scale = glm::vec3(size);
        mesh.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        mesh.position = glm::vec3(0.0f, 0.0, size);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    // back wall
    {
        rt::mesh mesh("../assets/models/quad.obj", 0);
        mesh.scale = glm::vec3(size);
        mesh.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
        mesh.position = glm::vec3(0.0f, 0.0, -size);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    // left wall
    {
        rt::mesh mesh("../assets/models/quad.obj", 1);
        mesh.scale = glm::vec3(size);
        mesh.rotation = glm::vec3(0.0f, 0.0f, 90.0f);
        mesh.position = glm::vec3(size, 0, 0.0f);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }

    // right wall
    {
        rt::mesh mesh("../assets/models/quad.obj", 2);
        mesh.scale = glm::vec3(size);
        mesh.rotation = glm::vec3(0.0f, 0.0f, -90.0f);
        mesh.position = glm::vec3(-size, 0, 0.0f);
        mesh.buildMatrix();
        renderer.scene.meshes.push_back(std::move(mesh));
    }
    
    renderer.render();

    while (!glfwWindowShouldClose(renderer.window)) {
        renderer.display();
    }

    renderer.terminate();

    return 0;
}