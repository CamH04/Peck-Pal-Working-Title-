#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

int main() {
	std::cout << "Peck Pal!";
	sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "Peck Pal");
    
    
    
    
    
    
    
    
    //lmao
    while (window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>())
                window.close();
        }
    }
	return 0;
}