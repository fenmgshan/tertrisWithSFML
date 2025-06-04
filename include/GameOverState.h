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

    sf::Font m_font;
    sf::Text m_scoreText;
    sf::Text m_retryButton;

    sf::Clock clock;
    float m_time = 0.0f;
    const float m_totalDuration = 4.0f;

    bool changeToTetrisGame = false;
};