#include "GameState.h"
#include "GameOverState.h"
#include "StartMenuState.h"

GameOverState::GameOverState(int score): finalScore(score){}
void GameOverState::handleEvent(sf::Event& event) {
    /*
    这一块是处理游戏结束状态的事件，比如点击重试按钮等
    wait for you to implement the event handling logic!
    */
}

void GameOverState::render(sf::RenderWindow& window) {
    /*
    这一块是渲染游戏结束状态，比如渲染分数和重试按钮等
    wait for you to implement the rendering logic!
    */
}

void GameOverState::update(std::unique_ptr<GameState>& currentState) {
    // 直接关闭了游戏，等待你的修改
    sf::Event closeEvent;
    closeEvent.type = sf::Event::Closed;
    handleEvent(closeEvent);
}
