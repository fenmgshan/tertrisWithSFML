#include "GameState.h"
#include "GameOverState.h"
#include "TetrisGame.h"
#include "StartMenuState.h"

GameOverState::GameOverState(int score) 
    : finalScore(score),
      m_scoreText(m_font, ""),
      m_retryButton(m_font, "")
    {
        if (!m_font.openFromFile("Aleo-Regular.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        m_scoreText.setFont(m_font);
        m_scoreText.setString("Final Score: " + std::to_string(finalScore));
        m_scoreText.setCharacterSize(24);
        m_scoreText.setFillColor(sf::Color::White);
        m_scoreText.setPosition(
            sf::Vector2f(WIDTH*TILE_SIZE/2.0 - m_scoreText.getGlobalBounds().position.x,
                        HEIGHT*TILE_SIZE/2.0 + m_scoreText.getGlobalBounds().position.y + 50)
        );

        m_retryButton.setFont(m_font);
        m_retryButton.setString("Retry");
        m_retryButton.setCharacterSize(50);
        m_retryButton.setFillColor(sf::Color::White);
        m_retryButton.setPosition(
            sf::Vector2f(WIDTH*TILE_SIZE/2.0 - m_retryButton.getGlobalBounds().position.x/2.0,
                        HEIGHT*TILE_SIZE/2.0 - m_retryButton.getGlobalBounds().position.y - 50)
        );
    }
void GameOverState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (event && event->is<sf::Event::MouseButtonPressed>()) {
        const auto& mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f textPos = m_retryButton.getPosition();
        sf::FloatRect bounds = m_retryButton.getGlobalBounds();

        if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            changeToTetrisGame = true;
        }
    }
}

void GameOverState::render(sf::RenderWindow& window) {

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
    sf::FloatRect retryButtonBounds = m_retryButton.getGlobalBounds();
    bool isHovered = retryButtonBounds.contains(static_cast<sf::Vector2f>(mousePos));

    if (isHovered) {
        m_retryButton.setFillColor(sf::Color::White);
        m_retryButton.setCharacterSize(60);
        m_retryButton.setOutlineColor(sf::Color::Yellow);
        m_retryButton.setOutlineThickness(5);
    } else {
        m_retryButton.setFillColor(sf::Color::White);
        m_retryButton.setCharacterSize(50);
        m_retryButton.setOutlineColor(sf::Color::Transparent);
        m_retryButton.setOutlineThickness(0);
    }

    window.draw(background);
    window.draw(m_scoreText);
    window.draw(m_retryButton);
}

void GameOverState::update(std::unique_ptr<GameState>& currentState) {
    if (changeToTetrisGame) {
        currentState = std::make_unique<TetrisGame>();
        changeToTetrisGame = false;
    }
}
