#include <array>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

constexpr int HEIGHT = 20;
constexpr int WIDTH = 10;
constexpr int BLOCK_SIZE = 4;
constexpr int TILE_SIZE = 50;
constexpr int WAIT_TIME = 500;

using Board = std::array<std::array<int, WIDTH>, HEIGHT>;
using Shape = std::array<std::array<int, BLOCK_SIZE>, BLOCK_SIZE>;



// 方块类
class Block {
public:
    int x, y;
    Shape shape;
    int type;
    Block () : x(WIDTH/2 - BLOCK_SIZE/2), y(0), type(0) {}

    // 初始化方块
    void initShape(int typeIdx, int rotations) {
        this->type = typeIdx;
        if (typeIdx < 0 || typeIdx >= static_cast<int>(shapes.size())) {
            typeIdx = 0;
            this->type = 0;
        }
        Shape final_roatated_shape = rotatedShape(shapes[typeIdx], rotations);
        this->shape = final_roatated_shape;
    }

    void rotateCurrentShape(int times) {
        Shape rotated = rotatedShape(this->shape, times);
        this->shape = rotated;
    }

    int getShapeAmount() const {
        return static_cast<int>(shapes.size());
    }

private:

    // 旋转
    static Shape rotatedShape(const Shape& src, int times) {
        Shape result_of_one_rotation;
        Shape current_shape_to_rotate = src;
        times = times % 4;
        if (times == 0) {
            return src;
        }

        for (auto& row : result_of_one_rotation) {
            row.fill(0);
        }
 
        for (int r=0; r<times; r++) {
           for (int i=0; i<BLOCK_SIZE; i++) {
                for (int j=0; j<BLOCK_SIZE; j++) {
                    result_of_one_rotation[j][BLOCK_SIZE-1-i] = current_shape_to_rotate[i][j];
                }
            }
            current_shape_to_rotate = result_of_one_rotation;
        }
        return current_shape_to_rotate;
    }

    // 方块种类
    static constexpr std::array<Shape, 7> shapes = {{
        // I shape
        {{
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        }},
        // J shape
        {{
            {0,0,0,0},
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0}
        }},
        // L shape
        {{
            {0,0,0,0},
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0}
        }},
        // O shape
        {{
            {0,0,0,0},
            {1,1,0,0},
            {1,1,0,0},
            {0,0,0,0}
        }},
        // S shape
        {{
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        }},
        // T shape
        {{
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }},
        // Z shape
        {{
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        }}
    }};
};

// 游戏逻辑
class TetrisGame {
public:
    TetrisGame() : 
        score (0), 
        window(sf::VideoMode(WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE), "Tetris"), 
        running(true),
        m_fontLoadSuccessful(false)
    {
        window.setFramerateLimit(60);
        if (!m_font.loadFromFile("Aleo-Regular.ttf")) {
            std::cerr << "Error: can't loading the font. Score will not display." << std::endl;
            m_fontLoadSuccessful = false;
        } else {
            m_fontLoadSuccessful = true;
            m_scoreText.setFont(m_font);
            m_scoreText.setCharacterSize(50);
            m_scoreText.setFillColor(sf::Color::White);
            m_scoreText.setPosition(10,10);
        }
        for (auto& row : board) {
            row.fill(0);
        }
        spawnNewBlockAndThread();
    }

    ~TetrisGame() {
        stopAndJoinAutoDropThread();
    }

    int getScore() const {
        return score;
    }

    const Board& getBoard() const {
        return board;
    }

    void run() {
        while (window.isOpen()) {
            if (!running) {
                break;
            }
            handleEvents();
            update();
            render();
        }
    }

    void reset() {
        for (auto& row : board) {
            row.fill(0);
        }
        score = 0;
    }

private:

    Board board;
    int score;

    sf::Font m_font;
    sf::Text m_scoreText;
    bool m_fontLoadSuccessful;

    sf::RenderWindow window;
    std::atomic<bool> running;

    Block m_currentBlock;

    std::mutex m_blockMutex;
    std::thread m_autoDropThread;

    void closeWindow() {
        bool expected = true;
        if (running.compare_exchange_strong(expected, false)) {
            if (m_autoDropThread.joinable()) {
                m_autoDropThread.join();
            }
        }
        if (window.isOpen()) {
            window.close();
        }
    }

    bool isWindowOpen() const {
        return window.isOpen();
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                closeWindow();
                return;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (!running) {
                    if (event.key.code == sf::Keyboard::R) {
                        if (m_autoDropThread.joinable()) {
                             m_autoDropThread.join();
                        }
                        reset();
                        running = true;
                        spawnNewBlockAndThread();
                    }
                } else { 
                    switch (event.key.code) {
                        case sf::Keyboard::Left:
                            moveLeft();
                            break;
                        case sf::Keyboard::Right:
                            moveRight();
                            break;
                        case sf::Keyboard::Down:
                            moveDown();
                            break;
                        case sf::Keyboard::Up:
                            rotate();
                            break;
                        case sf::Keyboard::Space:
                            {
                                std::lock_guard<std::mutex> lock(m_blockMutex);
                                if (!running) break;
                                while (!checkCollision_nolock(m_currentBlock.x, m_currentBlock.y + 1, m_currentBlock.shape)) {
                                    m_currentBlock.y++;
                                }
                            }
                            break;
                        case sf::Keyboard::R:
                            stopAndJoinAutoDropThread(); 
                            reset(); 
                            running = true;
                            spawnNewBlockAndThread();
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    bool checkCollision_nolock(int nx, int ny, const Shape& shape) const {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (shape[i][j]) {
                    int bx = nx + j;
                    int by = ny + i;
                    if (bx < 0 || bx >= WIDTH || by >= HEIGHT)
                        return true;
                    if (by >= 0 && board[by][bx])
                        return true;
                }
            }
        }
        return false;
    }

    void spawnNewBlockAndThread() {
        {
            std::lock_guard<std::mutex> lock(m_blockMutex);
            m_currentBlock.x = WIDTH/2 - BLOCK_SIZE/2;
            m_currentBlock.y = 0;
            m_currentBlock.initShape(rand() % m_currentBlock.getShapeAmount(), 0);
        }
        if (m_autoDropThread.joinable()) {
            m_autoDropThread.join();
        }
        m_autoDropThread = std::thread(&TetrisGame::auto_drop_task, this);
    }

    void stopAndJoinAutoDropThread() {
        running = false;
        if (m_autoDropThread.joinable()) {
            m_autoDropThread.join();
        }
    }

    void auto_drop_task() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
            if (!running) {
                break;
            }
            bool canMoveDown;
            {
                std::lock_guard<std::mutex> lock(m_blockMutex);
                if (!running) {
                    break;
                }
                canMoveDown = !checkCollision_nolock(m_currentBlock.x, m_currentBlock.y+1, m_currentBlock.shape);
                if (canMoveDown) {
                    m_currentBlock.y++;
                }
            }

            if (!canMoveDown) {
                break;
            }
        }
    }

    void render() {
        window.clear(sf::Color(50,50,50)); 
        {
            std::lock_guard<std::mutex> lock(m_blockMutex); 

            for (int i=0; i<HEIGHT; ++i) {
                for (int j=0; j<WIDTH; ++j) {
                    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE-1, TILE_SIZE-1));
                    tile.setPosition(static_cast<float>(j*TILE_SIZE), static_cast<float>(i*TILE_SIZE));
                    tile.setFillColor(numberToColor(board[i][j]));
                    if (board[i][j] == 0) { 
                        tile.setOutlineThickness(1);
                        tile.setOutlineColor(sf::Color(70,70,70));
                    }
                    window.draw(tile);
                }
            }
            
            if (running) { 
                sf::RectangleShape currentTile(sf::Vector2f(TILE_SIZE-1, TILE_SIZE-1));
                for (int r=0; r<BLOCK_SIZE; ++r) { 
                    for (int c=0; c<BLOCK_SIZE; ++c) { 
                        if (m_currentBlock.shape[r][c]) {
                            currentTile.setPosition(static_cast<float>((m_currentBlock.x+c) * TILE_SIZE),
                                                    static_cast<float>((m_currentBlock.y+r) * TILE_SIZE)); 
                            currentTile.setFillColor(numberToColor(m_currentBlock.type + 1));
                            currentTile.setOutlineThickness(0); 
                            window.draw(currentTile);
                        }
                    }
                }
            }
        } 

        if (m_fontLoadSuccessful) {
            m_scoreText.setString("Score: " + std::to_string(this->score));
            sf::FloatRect textRect = m_scoreText.getLocalBounds();
            m_scoreText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top);
            m_scoreText.setPosition(static_cast<float>(window.getSize().x)/2.0f, m_scoreText.getCharacterSize() + 10);
            window.draw(m_scoreText);
        }
        window.display();
    }

    sf::Color numberToColor(int number) const {
        switch (number) {
            case 0: return sf::Color(30,30,30); // Empty (Dark Gray)
            case 1: return sf::Color::Cyan; // I
            case 2: return sf::Color::Blue; // J
            case 3: return sf::Color(255,165,0); // L (Orange)
            case 4: return sf::Color::Yellow; // O
            case 5: return sf::Color::Green; // S
            case 6: return sf::Color::Magenta; // T
            case 7: return sf::Color::Red; // Z
            default: return sf::Color(30,30,30); // Default to Dark Gray for any unexpected number
        }
    }

    void clearLines() {
        int numLinesActuallyClearedThisTurn = 0;
        for (int i=HEIGHT-1; i>=0; ) {
            bool isRowFull = true;
            for (int j=0; j<WIDTH; ++j) {
                if (board[i][j] == 0) {
                    isRowFull = false;
                    break;
                }
            }
            if (isRowFull) {
                numLinesActuallyClearedThisTurn++;
                for (int k=i; k>0; --k) {
                    for (int l=0; l<WIDTH; ++l) {
                        board[k][l] = board[k-1][l];
                    }
                }
                for (int l=0; l<WIDTH; ++l) {
                    board[0][l] = 0;
                }
            } else {
                i--;
            }
        }
        if (numLinesActuallyClearedThisTurn > 0) {
            switch (numLinesActuallyClearedThisTurn) {
                case 1: score += 100; break;
                case 2: score += 300; break;
                case 3: score += 500; break;
                case 4: score += 800; break;
                default: break;
            }
        }
    }

    void update() {
        if (!running) return; 

        bool shouldProcessLanding = false;
        {
            std::lock_guard<std::mutex> lock(m_blockMutex);
            
            if (!running) return;

            if (checkCollision_nolock(m_currentBlock.x, m_currentBlock.y + 1, m_currentBlock.shape)) {
                shouldProcessLanding = true;
                mergeBlock_unlock(m_currentBlock.x, m_currentBlock.y, m_currentBlock.shape, m_currentBlock.type);
            }
        }

        if (shouldProcessLanding) {
            if (!running) { 
                return;     
            }
            
            clearLines(); 
            if (m_autoDropThread.joinable()) {
                m_autoDropThread.join();
            }

            Block tempBlockForSpawnCheck;
            tempBlockForSpawnCheck.initShape(rand() % tempBlockForSpawnCheck.getShapeAmount(), 0); 
            if (checkCollision_nolock(tempBlockForSpawnCheck.x, tempBlockForSpawnCheck.y, tempBlockForSpawnCheck.shape)) {
                closeWindow(); 
                return;        
            } 

            if (running) {
                spawnNewBlockAndThread();
            }
        }
    }

    void mergeBlock_unlock(int x, int y, const Shape& shape, int type) {
        for (int i=0; i<BLOCK_SIZE; i++) {
            for (int j=0; j<BLOCK_SIZE; j++) {
                if (shape[i][j]) {
                    int boardX = x + j;
                    int boardY = y + i;
                    if (boardY >= 0 && boardY < HEIGHT && boardX >= 0 && boardX < WIDTH) {
                        board[boardY][boardX] = type + 1; // +1 to avoid 0 for empty
                    }
                }
            }
        }
    }

    void moveDown() {
        std::lock_guard<std::mutex> lock(m_blockMutex);
        if (!running) return;
        if (!checkCollision_nolock(m_currentBlock.x, m_currentBlock.y + 1, m_currentBlock.shape)) {
            m_currentBlock.y++;
        } else {

        }
    }

    void moveLeft() {
        std::lock_guard<std::mutex> lock(m_blockMutex);
        if (!running) return;
        if (!checkCollision_nolock(m_currentBlock.x - 1, m_currentBlock.y, m_currentBlock.shape)) {
            m_currentBlock.x--;
        }
    }
    
    void moveRight() {
        std::lock_guard<std::mutex> lock(m_blockMutex);
        if (!running) return;
        if (!checkCollision_nolock(m_currentBlock.x + 1, m_currentBlock.y, m_currentBlock.shape)) {
            m_currentBlock.x++;
        }
    }

    void rotate() {
        std::lock_guard<std::mutex> lock(m_blockMutex);
        if (!running) {
            return;
        }

        Shape originalShape = m_currentBlock.shape;
        int originalX = m_currentBlock.x;
        int originalY = m_currentBlock.y;

        m_currentBlock.rotateCurrentShape(1);

        // 首先检查旋转后在原位置是否碰撞
        if (!checkCollision_nolock(m_currentBlock.x, m_currentBlock.y, m_currentBlock.shape)) {
            return; // 没有碰撞，旋转成功，无需踢墙
        }

        // 如果在原位置碰撞，则尝试一系列的“踢墙”位移
        std::vector<std::pair<int, int>> kicks; // 定义一个存储踢墙位移 (dx, dy) 的列表

        if (m_currentBlock.type == 0) {
            kicks = {
                // 水平踢墙: 优先尝试较小的位移
                {-1, 0}, {1, 0},   
                {-2, 0}, {2, 0},   
                // 垂直踢墙: I型方块的垂直踢墙尤其重要
                {0, -1}, 
                {0, -2}, 
                {0, 1}, 
                {0, 2} 
            };
        } else { // 其他类型方块 (J, L, S, T, Z, O) 的踢墙规则
            kicks = {
                // 水平踢墙
                {-1, 0}, {1, 0}, 
                {-2, 0}, {2, 0}, 
                // 垂直踢墙
                {0, -1},         
                {0, 1}           
            };
        }

        // 遍历定义的踢墙位移列表
        for (const auto& kick : kicks) {
            int kickedX = originalX + kick.first; 
            int kickedY = originalY + kick.second;

            if (!checkCollision_nolock(kickedX, kickedY, m_currentBlock.shape)) {
                m_currentBlock.x = kickedX;
                m_currentBlock.y = kickedY;
                return; 
            }
        }

        m_currentBlock.shape = originalShape;
        m_currentBlock.x = originalX;
        m_currentBlock.y = originalY;
    }

};

int main() {
    srand(static_cast<unsigned int>(time(0)));
    TetrisGame game;
    game.run();
    return 0;
} 