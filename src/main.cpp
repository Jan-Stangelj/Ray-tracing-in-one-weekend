#include <glm/glm.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

#include "ray.hpp"
#include "camera.hpp"

#include <iostream>

void rayTrace(sf::Image& resoult, rt::camera cam);

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ray tracing in one weekend", sf::Style::Titlebar | sf::Style::Close);

    sf::Image resoult({800, 600});
    sf::Texture resoultTexture;

    rt::camera cam(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), 800, 600, 60.0f);

    auto start = std::chrono::high_resolution_clock::now();

    rayTrace(resoult, cam);


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "Rendering took: " << ms.count() << " ms\n";

    if (!resoultTexture.loadFromImage(resoult))
        std::cerr << "Failed to load resoult texture.\n";

    sf::Sprite resoultSprite(resoultTexture);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        window.draw(resoultSprite);

        window.display();
    }

    return 0;
}

bool raySphere(rt::ray r, glm::vec3 center, float radius) {
    glm::vec3 oc = r.origin() - center;
    float a = glm::dot(r.direction(), r.direction());
    float half_b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    return (half_b*half_b - a*c) >= 0;
}

void rayTrace(sf::Image& resoultImage, rt::camera cam) {
    for (unsigned int y = 0; y < cam.resolutionY(); y++) {
        for (unsigned int x = 0; x < cam.resolutionX(); x++) {
            glm::vec3 resoult(0.0f);

            rt::ray r = cam.genRay(x, y);

            float skyVar = (glm::normalize(r.direction()).y + 1) / 2;
            glm::vec3 skyColur(0.5f, 0.7f, 1.0f);
            glm::vec3 groundColur(1.0f);

            resoult = glm::vec3(skyVar * groundColur + (1.0f-skyVar) * skyColur);

            if (raySphere(r, glm::vec3(0.0f, 0.0f, 3.0f), 0.5f))
                resoult = glm::vec3(1.0f);

            resoultImage.setPixel({x, y}, sf::Color(resoult.r*255, resoult.g*255, resoult.b*255, 255));
        }
    }
}