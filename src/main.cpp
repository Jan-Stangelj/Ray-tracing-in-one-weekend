#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

#include <glm/glm.hpp>
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include "random.hpp"
#include "ray.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "random"

#include <iostream>

rt::hitInfo traceRay(rt::ray r, const rt::scene& scene) {
    rt::hitInfo hit;

    if (!scene.hit(r, hit)) {
        float skyVar = (glm::normalize(r.direction()).y + 1) / 2;

        glm::vec3 skyColur(0.5f, 0.7f, 1.0f);
        glm::vec3 groundColur(1.0f);

        hit.solidAdd = glm::vec3(skyVar * skyColur + (1.0f-skyVar) * groundColur);
    }

    return hit;
}

glm::vec3 perSample(rt::ray r, const rt::scene& scene, uint32_t& seed) {
    glm::vec3 resoult(1.0f);

    rt::hitInfo hit;

    uint32_t bounces = 12;

    for (unsigned int i = 0; i < bounces; i++) {
        hit = traceRay(r, scene);

        if (hit.sphere == -1) {
            return resoult * hit.solidAdd;
        }
        if (scene.spheres.at(hit.sphere).getEmission() != glm::vec3(0.0f)) {
            return resoult * scene.spheres.at(hit.sphere).getEmission();
        }

        resoult *= scene.spheres.at(hit.sphere).getColur();

        glm::vec3 newOrigin = hit.origin + hit.normal * 0.0001f;
        glm::vec3 newDirection = glm::normalize(hit.normal + rt::randomUnitVec3(seed));

        r = rt::ray(newOrigin, newDirection);
    }

    return resoult;
}

void render(sf::Image& resoultImage, const rt::camera& cam, const rt::scene& scene) {
    for (unsigned int y = 0; y < cam.resolutionY(); y++) {
        for (unsigned int x = 0; x < cam.resolutionX(); x++) {

            unsigned int samples = 1024;
            float jiggle = 0.001f;

            rt::ray ray = cam.genRay(x, y);

            glm::vec3 resoult(0.0f);

            uint32_t seed = rt::PCGhash(x) + rt::PCGhash(y);

            for (unsigned int i = 0; i < samples; i++) {
                rt::ray r(ray.origin(), glm::normalize(ray.direction() + rt::randomUnitVec3(seed)*jiggle));
                resoult += perSample(r, scene, seed);
            }

            resoult /= samples;

            resoult = glm::clamp(resoult, 0.0f, 1.0f);

            resoultImage.setPixel({x, y}, sf::Color(resoult.r*255, resoult.g*255, resoult.b*255, 255));
        }

        std::cout << floor((float)y / (float)cam.resolutionY() * 100.0f) << "% completed\n";
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ray tracing in one weekend", sf::Style::Titlebar | sf::Style::Close);

    rt::camera cam(glm::vec3(0.0f, 1.0f, -3.0f), glm::vec3(0.0f), 800, 600, 60.0f);

    rt::scene scene;
    scene.spheres.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.3f), glm::vec3(0.0f));
    scene.spheres.emplace_back(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, glm::vec3(0.5f), glm::vec3(0.0f));
    scene.spheres.emplace_back(glm::vec3(1.5f, 1.0f, 1.5f), 0.5f, glm::vec3(0.0f), glm::vec3(25.0f));

    auto start = std::chrono::high_resolution_clock::now();

    sf::Image resoult({800, 600});
    sf::Texture resoultTexture;
    render(resoult, cam, scene);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "Rendering took: " << ms.count() << " ms\n";

    if (!resoultTexture.loadFromImage(resoult))
        std::cerr << "Failed to load resoult texture.\n";
    sf::Sprite resoultSprite(resoultTexture);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
            if (event->is<sf::Event::Closed>())
                window.close();

        window.clear(sf::Color::Black);
        window.draw(resoultSprite);
        window.display();
    }

    return 0;
}