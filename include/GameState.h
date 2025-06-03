#pragma once
#include "Constants.h"
#include <memory>

class GameState {
public:
    virtual void handleEvent(sf::Event& event) = 0;
    virtual void update(std::unique_ptr<GameState>& currentState) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual ~GameState() = default;
};