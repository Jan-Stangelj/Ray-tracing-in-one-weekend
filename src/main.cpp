#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "raytracing.hpp"
#include "scene.hpp"
#include "settings.hpp"

#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({rt::resolutionX, rt::resolutionY}), rt::title, sf::Style::Titlebar | sf::Style::Close);

    rt::camera cam(glm::vec3(0.0f, 1.0f, -3.0f), glm::vec3(0.0f), rt::resolutionX, rt::resolutionY, rt::fovY);

    rt::scene scene;
    scene.spheres.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.3f), glm::vec3(0.0f));
    scene.spheres.emplace_back(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, glm::vec3(0.5f), glm::vec3(0.0f));
    scene.spheres.emplace_back(glm::vec3(1.5f, 1.0f, -1.0f), 0.5f, glm::vec3(0.0f), glm::vec3(10.0f));

    auto start = std::chrono::high_resolution_clock::now();

    sf::Image resoult({rt::resolutionX, rt::resolutionY});
    sf::Texture resoultTexture;
    rt::render(resoult, cam, scene);

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