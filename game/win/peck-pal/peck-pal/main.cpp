#include <iostream>
#include <windows.h>
#include <set>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


//keyboard hook
int* g_counter = nullptr;
sf::Text* g_counterText = nullptr;
sf::RectangleShape* g_box = nullptr;
std::set<DWORD> g_pressedKeys;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && g_counter && g_counterText && g_box) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyboard->vkCode;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            if (g_pressedKeys.find(vkCode) == g_pressedKeys.end()) {
                g_pressedKeys.insert(vkCode);
                (*g_counter)++;
                g_counterText->setString(std::to_string(*g_counter));
                sf::FloatRect bounds = g_counterText->getLocalBounds();
                sf::Vector2f origin(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);
                g_counterText->setOrigin(origin);
                sf::Vector2f boxCenter = g_box->getPosition() + g_box->getSize() / 2.f;
                g_counterText->setPosition(boxCenter);
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            g_pressedKeys.erase(vkCode);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 7);
    std::cout << "=== SFML version: " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << " === \n";
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
    g_counter = &counter;
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
    g_box = &box;

    sf::Sprite sprite(texture);
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 512, 512 }));
    sprite.setScale({ 0.2f, 0.2f });
    auto bounds = sprite.getGlobalBounds();
    sprite.setPosition(sf::Vector2f(
        (window.getSize().x - bounds.size.x) / 2.f,
        box.getPosition().y - bounds.size.y + 10.f
    ));
    sf::Font font("Papernotes.ttf");
    SetConsoleTextAttribute(h, 10);
    std::cout << "Loaded Font\n";
    SetConsoleTextAttribute(h, 7);
    sf::Text counterText(font, "0", 32);
    counterText.setFillColor(sf::Color::Black);
    g_counterText = &counterText;
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

    //hook install
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (!keyboardHook) {
        SetConsoleTextAttribute(h, 12);
        std::cerr << "Failed to install keyboard hook\n";
        SetConsoleTextAttribute(h, 7);
        return -1;
    }
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Right) { //right click drags window
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
    g_pressedKeys.clear();
    UnhookWindowsHookEx(keyboardHook);
    SetConsoleTextAttribute(h, 7);
    return 0;
}