#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>

#include <iostream>

void rayTrace(sf::Image& resoult);

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Ray tracing in one weekend", sf::Style::Titlebar | sf::Style::Close);

    sf::Image resoult({800, 600});
    sf::Texture resoultTexture;

    rayTrace(resoult);

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

void rayTrace(sf::Image& resoult) {
    for (unsigned int y = 0; y < 600; y++) {
        for (unsigned int x = 0; x < 800; x++) {
            float rf = y/600.0f;
            float gf = x/800.0f;

            resoult.setPixel({x, y}, sf::Color(rf*255, gf*255, 0, 255));
        }
    }
}