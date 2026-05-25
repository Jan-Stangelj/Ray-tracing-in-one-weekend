#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

#include <cstdint>
#include <glm/glm.hpp>

#include <iostream>

struct camera {
    glm::mat4 vpInv;
    uint32_t width;
    uint32_t height;
};

void rayTrace(sf::Image& resoult, camera cam);

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ray tracing in one weekend", sf::Style::Titlebar | sf::Style::Close);

    sf::Image resoult({800, 600});
    sf::Texture resoultTexture;

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), 800.0f/600.0f, 0.1f, 100.0f);
    glm::mat4 vp = proj * view;
    glm::mat4 vpInv = glm::inverse(vp);

    camera cam(vpInv, 800, 600);


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

struct ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

ray genRay(uint32_t x, uint32_t y, const camera& cam) {
    float ndcX = (x + 0.5f) / cam.width * 2 - 1;
    float ndcY = (y + 0.5f) / cam.height * 2 - 1;

    glm::vec4 nearClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farClip = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    glm::vec4 nearWorld = cam.vpInv * nearClip;
    glm::vec4 farWorld = cam.vpInv * farClip;

    nearWorld /= nearWorld.w;
    farWorld /= farWorld.w;

    return ray(glm::vec3(nearWorld), glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld)));
}

bool raySphere(ray r, glm::vec3 center, float radius) {
    glm::vec3 oc = r.origin - center;
    float a = glm::dot(r.direction, r.direction);
    float half_b = dot(oc, r.direction);
    float c = dot(oc, oc) - radius * radius;
    return (half_b*half_b - a*c) >= 0;
}

void rayTrace(sf::Image& resoultImage, camera cam) {
    for (unsigned int y = 0; y < 600; y++) {
        for (unsigned int x = 0; x < 800; x++) {
            glm::vec3 resoult(0.0f);

            ray r = genRay(x, y, cam);

            if (raySphere(r, glm::vec3(0.0f, 0.0f, 3.0f), 0.5f))
                resoult = glm::vec3(1.0f);

            resoultImage.setPixel({x, y}, sf::Color(resoult.r*255, resoult.g*255, resoult.b*255, 255));
        }
    }
}