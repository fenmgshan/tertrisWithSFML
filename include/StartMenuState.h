#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>

class StartMenuState: public GameState {
public:
    StartMenuState(sf::RenderWindow& window);
    void handleEvent(sf::Event& event) override;
    void update(std::unique_ptr<GameState>& currentState) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Font font;
    sf::Text title;
    sf::Text startButton;
};
