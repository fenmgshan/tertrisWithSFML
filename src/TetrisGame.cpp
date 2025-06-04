#include "Constants.h"
#include "GameOverState.h"
#include "TetrisGame.h"

TetrisGame::TetrisGame() : 
    score (0), 
    running(true),
    m_fontLoadSuccessful(false),
    m_scoreText(m_font, "", 50)
{
    if (!m_font.openFromFile("Aleo-Regular.ttf")) {
        std::cerr << "Error: can't loading the font. Score will not display." << std::endl;
        m_fontLoadSuccessful = false;
    } else {
        m_fontLoadSuccessful = true;
        m_scoreText.setFont(m_font);
        m_scoreText.setCharacterSize(50);
        m_scoreText.setFillColor(sf::Color::White);
        m_scoreText.setPosition(sf::Vector2f(10,10));
    }
    for (auto& row : board) {
        row.fill(0);
    }
    spawnNewBlockAndThread();
}

TetrisGame::~TetrisGame() {
    stopAndJoinAutoDropThread();
}

void TetrisGame::render(sf::RenderWindow& window) {
    window.clear(sf::Color(50,50,50)); 
    {
        std::lock_guard<std::mutex> lock(m_blockMutex); 

        for (int i=0; i<HEIGHT; ++i) {
            for (int j=0; j<WIDTH; ++j) {
                sf::RectangleShape tile(sf::Vector2f(TILE_SIZE-1, TILE_SIZE-1));
                tile.setPosition(sf::Vector2f(j*TILE_SIZE, i*TILE_SIZE));
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
                        currentTile.setPosition(sf::Vector2f((m_currentBlock.x+c) * TILE_SIZE,
                                                (m_currentBlock.y+r) * TILE_SIZE)); 
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
        m_scoreText.setOrigin(
            sf::Vector2f(
                textRect.position.x + textRect.size.x /2.0f,
                textRect.position.y
            )
        );
        m_scoreText.setPosition(
            sf::Vector2f(
                window.getSize().x / 2.0f, 
                m_scoreText.getCharacterSize() + 10
            )
        );
        window.draw(m_scoreText);
    }
    window.display();
}

void TetrisGame::handleEvent(const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
        if (!running) {
            if (keyPress->code == sf::Keyboard::Key::R) {
                if (m_autoDropThread.joinable()) {
                        m_autoDropThread.join();
                }
                reset();
                running = true;
                spawnNewBlockAndThread();
            }
        } else { 
            switch (keyPress->code) {
                case sf::Keyboard::Key::Left:
                    moveLeft();
                    break;
                case sf::Keyboard::Key::Right:
                    moveRight();
                    break;
                case sf::Keyboard::Key::Down:
                    moveDown();
                    break;
                case sf::Keyboard::Key::Up:
                    rotate();
                    break;
                case sf::Keyboard::Key::Space:
                    {
                        std::lock_guard<std::mutex> lock(m_blockMutex);
                        if (!running) break;
                        while (!checkCollision_nolock(m_currentBlock.x, m_currentBlock.y + 1, m_currentBlock.shape)) {
                            m_currentBlock.y++;
                        }
                    }
                    break;
                case sf::Keyboard::Key::R:
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


void TetrisGame::update(std::unique_ptr<GameState>& currentState) {
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
            currentState = std::make_unique<GameOverState>(score);
            return;        
        } 

        if (running) {
            spawnNewBlockAndThread();
        }
    }
}


void TetrisGame::reset() {
    for (auto& row : board) {
        row.fill(0);
    }
    score = 0;
}

bool TetrisGame::checkCollision_nolock(int nx, int ny, const Shape& shape) const {
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

void TetrisGame::spawnNewBlockAndThread() {
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

void TetrisGame::stopAndJoinAutoDropThread() {
    running = false;
    if (m_autoDropThread.joinable()) {
        m_autoDropThread.join();
    }
}

void TetrisGame::auto_drop_task() {
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

sf::Color TetrisGame::numberToColor(int number) const {
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

void TetrisGame::clearLines() {
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

void TetrisGame::mergeBlock_unlock(int x, int y, const Shape& shape, int type) {
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

void TetrisGame::moveDown() {
    std::lock_guard<std::mutex> lock(m_blockMutex);
    if (!running) return;
    if (!checkCollision_nolock(m_currentBlock.x, m_currentBlock.y + 1, m_currentBlock.shape)) {
        m_currentBlock.y++;
    } else {

    }
}

void TetrisGame::moveLeft() {
    std::lock_guard<std::mutex> lock(m_blockMutex);
    if (!running) return;
    if (!checkCollision_nolock(m_currentBlock.x - 1, m_currentBlock.y, m_currentBlock.shape)) {
        m_currentBlock.x--;
    }
}

void TetrisGame::moveRight() {
    std::lock_guard<std::mutex> lock(m_blockMutex);
    if (!running) return;
    if (!checkCollision_nolock(m_currentBlock.x + 1, m_currentBlock.y, m_currentBlock.shape)) {
        m_currentBlock.x++;
    }
}

void TetrisGame::rotate() {
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

