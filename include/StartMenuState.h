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
    sf::Font m_font;
    sf::Text m_titleText;
    sf::Text m_startButton;

    sf::Clock clock;
    float m_time = 0.0f;
    const float m_totalDuration = 4.0f; 

    bool changeToTetrisGame = false;
};
