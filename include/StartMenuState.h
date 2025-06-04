#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>

class StartMenuState: public GameState {
public:
    StartMenuState(sf::RenderWindow& window);
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void update(std::unique_ptr<GameState>& currentState) override;
    void render(sf::RenderWindow& window) override;

private:
};
