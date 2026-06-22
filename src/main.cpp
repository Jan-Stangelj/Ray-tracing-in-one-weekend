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

void rayTrace(sf::Image& resoult, const rt::camera& cam, const rt::scene& scene);

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ray tracing in one weekend", sf::Style::Titlebar | sf::Style::Close);

    rt::camera cam(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f), 800, 600, 60.0f);

    rt::scene scene;
    scene.spheres.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, glm::vec3(1.0f));
    scene.spheres.emplace_back(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, glm::vec3(1.0f));

    auto start = std::chrono::high_resolution_clock::now();

    sf::Image resoult({800, 600});
    sf::Texture resoultTexture;
    rayTrace(resoult, cam, scene);

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

void rayTrace(sf::Image& resoultImage, const rt::camera& cam, const rt::scene& scene) {
    for (unsigned int y = 0; y < cam.resolutionY(); y++) {
        for (unsigned int x = 0; x < cam.resolutionX(); x++) {

            unsigned int samples = 10;
            float jiggle = 0.001f;

            rt::ray ray = cam.genRay(x, y);

            glm::vec3 resoult(0.0f);

            for (unsigned int i = 0; i < samples; i++) {
                uint32_t seed = x*y*i;

                rt::ray r(ray.origin(), ray.direction() + rt::randomUnitVec3(seed)*jiggle);
                rt::hitInfo hit;

                if (!scene.hit(r, hit)) {
                    float skyVar = (glm::normalize(r.direction()).y + 1) / 2;

                    glm::vec3 skyColur(0.5f, 0.7f, 1.0f);
                    glm::vec3 groundColur(1.0f);

                    resoult += glm::vec3(skyVar * skyColur + (1.0f-skyVar) * groundColur);
                    continue;
                }

                resoult += hit.normal * 0.5f + 0.5f;
            }

            resoult /= samples;

            resoultImage.setPixel({x, y}, sf::Color(resoult.r*255, resoult.g*255, resoult.b*255, 255));
        }

        std::cout << floor((float)y / (float)cam.resolutionY() * 100.0f) << "% completed\n";
    }
}