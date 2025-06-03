#pragma once
#include "Constants.h"
#include "GameState.h"
#include "Block.h"

using Board = std::array<std::array<int, WIDTH>, HEIGHT>;

class TetrisGame: public GameState {
public:
    TetrisGame();
    ~TetrisGame();

    void reset();
    void render(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update(std::unique_ptr<GameState>& currentState) override;

private:
    Board board;
    int score;

    sf::Font m_font;
    sf::Text m_scoreText;
    bool m_fontLoadSuccessful;

    std::atomic<bool> running;

    Block m_currentBlock;

    std::mutex m_blockMutex;
    std::thread m_autoDropThread;


    bool checkCollision_nolock(int nx, int ny, const Shape& shape) const;
    void spawnNewBlockAndThread();
    void stopAndJoinAutoDropThread();
    void auto_drop_task();
    sf::Color numberToColor(int number) const;
    void clearLines();
    void mergeBlock_unlock(int x, int y, const Shape& shape, int type);
    void moveDown();
    void moveLeft();
    void moveRight();
    void rotate();
};