#include "GameState.h"
#include "StartMenuState.h"
#include "TetrisGame.h"
#include <SFML/Graphics.hpp>

StartMenuState::StartMenuState(sf::RenderWindow& window)
    : 
      m_titleText(m_font, ""),
      m_startButton(m_font, ""),
      m_time(0.0f),
      m_totalDuration(4.0f),
      changeToTetrisGame(false) 
{
    if (!m_font.openFromFile("Aleo-Regular.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    m_titleText.setFont(m_font);
    m_titleText.setString("TETRIS");
    m_titleText.setCharacterSize(80);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition(
        sf::Vector2f(100.0,
                    window.getSize().y / 3.0)
    );

    m_startButton.setFont(m_font);
    m_startButton.setString("Start Game");
    m_startButton.setCharacterSize(50);
    m_startButton.setFillColor(sf::Color::White);
    m_startButton.setPosition(
        sf::Vector2f(100.0,
                    window.getSize().y / 2.0)
    );

}

void StartMenuState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (event && event->is<sf::Event::MouseButtonPressed>()) {
        const auto& mousePos = sf::Mouse::getPosition(window);
        
        if (m_startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            changeToTetrisGame = true;
        }
        
    }
}

void StartMenuState::update(std::unique_ptr<GameState>& currentState) {
    if (changeToTetrisGame) {
        currentState = std::make_unique<TetrisGame>();
        changeToTetrisGame = false;
    }
}

void StartMenuState::render(sf::RenderWindow& window) {
    m_time += clock.restart().asSeconds();
    float progress = fmod(m_time, m_totalDuration) / m_totalDuration;
    float red;
    if (progress < 0.5f) {
        red = 170.0f * (progress / 0.5f);
    } else {
        red = 170.0f * (2.0f - (progress / 0.5f));
    }

    sf::RectangleShape background(sf::Vector2f(window.getSize()));
    background.setFillColor(sf::Color(red, 0, 0, 255));

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    bool isStartHovered = m_startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));

    if (isStartHovered) {
        m_startButton.setFillColor(sf::Color::White);
        m_startButton.setCharacterSize(60);
        m_startButton.setOutlineColor(sf::Color::Yellow);
        m_startButton.setOutlineThickness(5);
    } else {
        m_startButton.setFillColor(sf::Color::White);
        m_startButton.setCharacterSize(50);
        m_startButton.setOutlineColor(sf::Color::Transparent);
        m_startButton.setOutlineThickness(0);
    }

    window.draw(background);
    window.draw(m_titleText);
    window.draw(m_startButton);
}
