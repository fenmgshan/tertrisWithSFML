#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>

class GameOverState : public GameState {
public:
    GameOverState(int score);
    void handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;
    void update(std::unique_ptr<GameState>& currentState) override;

private:
    int finalScore;
};