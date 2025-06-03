#pragma once
#include "GameState.h"
#include <SFML/Graphics.hpp>

class GameOverState : public GameState {
public:
    GameOverState(int score);
    void handleEvent(sf::Event& event) override;
    void render(sf::RenderWindow& window) override;
    void update(std::unique_ptr<GameState>& currentState) override;

private:
    int finalScore;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text retryButton;
};