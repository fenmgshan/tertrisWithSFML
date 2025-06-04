#include "GameState.h"
#include "StartMenuState.h"
#include "TetrisGame.h"

StartMenuState::StartMenuState(sf::RenderWindow& window){}

void StartMenuState::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    /*
    这一块是处理开始菜单的事件，比如点击开始按钮等
    wait for you to implement the event handling logic!
    */
}
void StartMenuState::update(std::unique_ptr<GameState>& currentState) {
    // 直接进入游戏状态，等待你的后续修改。
    currentState = std::make_unique<TetrisGame>();
}
void StartMenuState::render(sf::RenderWindow& window) {
    /*
    这一块是渲染开始菜单，比如渲染标题和开始按钮等
    wait for you to implement the rendering logic!
    */
}
