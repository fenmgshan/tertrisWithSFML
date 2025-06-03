#include "Constants.h"
#include "Block.h"
#include "GameState.h"
#include "GameOverState.h"
#include "StartMenuState.h"
#include "TetrisGame.h"

int main() {
    srand(static_cast<unsigned int>(time(0)));
    sf::RenderWindow window(sf::VideoMode(WIDTH*TILE_SIZE, HEIGHT*TILE_SIZE), "Tetris");
    window.setFramerateLimit(60);
    
    std::unique_ptr<GameState> currentState = std::make_unique<StartMenuState>(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            currentState->handleEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }
        currentState->update(currentState);
        window.clear();
        currentState->render(window);
        window.display();
    }
    return 0;
}