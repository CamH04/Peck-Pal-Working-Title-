#include <iostream>
#include <windows.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

int main() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 7);
    std::cout << "=== SFML version: "
        << SFML_VERSION_MAJOR << "."
        << SFML_VERSION_MINOR << "."
        << SFML_VERSION_PATCH << " === \n";
    std::cout << "Started:";
    SetConsoleTextAttribute(h, 13);
    std::cout << " Peck Pal!\n";
    sf::RenderWindow window(sf::VideoMode({ 200, 400 }), "Peck Pal", sf::Style::None);

    // making window transparent and clean (dark grey alpha as transparency) 
    // i hate windows
    HWND hwnd = FindWindowA(NULL, "Peck Pal");
    if (hwnd) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd, RGB(1, 1, 1), 0, LWA_COLORKEY);
    }
    int counter = 0;
    bool isDragging = false;
    sf::Vector2i dragOffset;

    sf::Texture texture;
    if (!texture.loadFromFile("sprites/barnowl.png")) {
        std::cerr << "Failed to load texture\n";
        return -1;
    }
    SetConsoleTextAttribute(h, 10);
    std::cout << "Loaded Sprite\n";
    SetConsoleTextAttribute(h, 7);

    sf::RectangleShape box({ 180.f, 100.f });
    box.setFillColor(sf::Color(227, 218, 201));
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(3.f);
    box.setPosition({ 10.f, 300.f });

    sf::Sprite sprite(texture);
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 512, 512 }));
    sprite.setScale({ 0.2f, 0.2f });
    auto bounds = sprite.getGlobalBounds();
    sprite.setPosition(sf::Vector2f(
        (window.getSize().x - bounds.size.x) / 2.f,
        box.getPosition().y - bounds.size.y - 5.f
    ));

    sf::Font font("Papernotes.ttf");
    SetConsoleTextAttribute(h, 10);
    std::cout << "Loaded Font\n";
    SetConsoleTextAttribute(h, 7);

    sf::Text counterText(font, "0", 32);
    counterText.setFillColor(sf::Color::Black);

    SetConsoleTextAttribute(h, 10);
    std::cout << "Drawn Box\n";
    SetConsoleTextAttribute(h, 7);

    auto centerTextInBox = [&]() {
        sf::FloatRect bounds = counterText.getLocalBounds();
        sf::Vector2f origin(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        );
        counterText.setOrigin(origin);
        sf::Vector2f boxCenter = box.getPosition() + box.getSize() / 2.f;
        counterText.setPosition(boxCenter);
        };
    centerTextInBox();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) { //left click incremement
                if (mouse->button == sf::Mouse::Button::Left) {
                    counter++;
                    counterText.setString(std::to_string(counter));
                    centerTextInBox();
                }
                if (mouse->button == sf::Mouse::Button::Right) { //right lcick drags window
                    isDragging = true;
                    dragOffset = sf::Mouse::getPosition() - window.getPosition();
                }
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouse->button == sf::Mouse::Button::Right) {
                    isDragging = false;
                }
            }
        }
        if (isDragging) {
            sf::Vector2i newPos = sf::Mouse::getPosition() - dragOffset;
            window.setPosition(newPos);
        }
        window.clear(sf::Color(1, 1, 1));
        window.draw(sprite);
        window.draw(box);
        window.draw(counterText);
        window.display();
    }
    SetConsoleTextAttribute(h, 7);
    return 0;
}