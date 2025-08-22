#include "world/Map.h"
#include "core/RNG.h"
#include "Constants.h"
#include <fstream>
#include <queue>
#include <iostream>

Map::Map()
    : m_width(Constants::MAP_WIDTH)
    , m_height(Constants::MAP_HEIGHT)
    , m_startPos(Constants::START_X, Constants::START_Y)
    , m_goalPos(Constants::GOAL_X, Constants::GOAL_Y)
{
    m_tileShape.setSize(sf::Vector2f(Constants::TILE_SIZE, Constants::TILE_SIZE));
    initializeTiles();
}

Map::~Map() {
}

void Map::generateMaze(RNG& rng, unsigned int seed) {
    if (seed != 0) {
        rng.setSeed(seed);
    }

    // Clear previous data
    m_rocks.clear();
    m_teleportGates.clear();
    m_monsterPositions.clear();

    // Generate maze-like layout
    initializeTiles();
    generateMazeWalls(rng);
    createMazePath(rng);
    placeMandatoryTiles();
    placeMonsters(rng);
    placeRocks(rng);
    setupTeleportGates(rng);
}

void Map::generateMap(RNG& rng, unsigned int seed) {
    // Legacy method - use enhanced maze generation
    generateMaze(rng, seed);
}
void Map::generateZigZag(RNG& rng, unsigned int seed) {
    if (seed != 0) rng.setSeed(seed);

    // Reset map to all walls (0)
    initializeTiles();
    for (int y = 0; y < m_height; ++y)
        for (int x = 0; x < m_width; ++x)
            setTileType(x, y, TileType::Wall);

    // Zigzag từ ô (0,0) tới ô (m_width-1, m_height-1)
    // 0 = tường, 1 = đường đi, 2 = điểm bắt đầu/kết thúc

    m_startPos = Vec2i(0, 0);
    m_goalPos = Vec2i(m_width - 1, m_height - 1);

    // Carve zigzag path từ (0,0) đến (width-1, height-1)
    int y = 0;
    bool leftToRight = true;

    while (y < m_height) {
        if (leftToRight) {
            // Đi từ trái sang phải
            for (int x = 0; x < m_width; ++x) {
                setTileType(x, y, TileType::Empty);
            }
        } else {
            // Đi từ phải sang trái
            for (int x = m_width - 1; x >= 0; --x) {
                setTileType(x, y, TileType::Empty);
            }
        }
        y += 1;
        leftToRight = !leftToRight;
    }

    // Set start và goal tiles
    setTileType(m_startPos, TileType::Empty);  // Start có thể đứng được
    setTileType(m_goalPos, TileType::Goal);    // Goal để nhận diện kết thúc

    // Zigzag thuần: không thêm obstacles để đường đi liền mạch
}

void Map::createZigZagPath() {
    // Helper kept for compatibility; generateZigZag does full job.
}


bool Map::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << filename << std::endl;
        return false;
    }

    // Simple CSV loading implementation
    std::string line;
    int y = 0;
    while (std::getline(file, line) && y < m_height) {
        for (int x = 0; x < m_width && x < static_cast<int>(line.length()); ++x) {
            if (x < static_cast<int>(line.length())) {
                m_tiles[y][x].setType(charToTileType(line[x]));
            }
        }
        y++;
    }

    return true;
}

bool Map::saveToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to create map file: " << filename << std::endl;
        return false;
    }

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            file << getTileChar(m_tiles[y][x].getType());
        }
        file << std::endl;
    }

    return true;
}

TileType Map::getTileType(int x, int y) const {
    if (!isValidPosition(x, y)) {
        return TileType::Rock; // Treat out-of-bounds as rock
    }
    return m_tiles[y][x].getType();
}

TileType Map::getTileType(const Vec2i& pos) const {
    return getTileType(pos.x, pos.y);
}

void Map::setTileType(int x, int y, TileType type) {
    if (isValidPosition(x, y)) {
        m_tiles[y][x].setType(type);
    }
}

void Map::setTileType(const Vec2i& pos, TileType type) {
    setTileType(pos.x, pos.y, type);
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

bool Map::isValidPosition(const Vec2i& pos) const {
    return isValidPosition(pos.x, pos.y);
}

bool Map::isWalkable(int x, int y) const {
    if (!isValidPosition(x, y)) {
        return false;
    }
    return m_tiles[y][x].isWalkable();
}

bool Map::isWalkable(const Vec2i& pos) const {
    return isWalkable(pos.x, pos.y);
}

bool Map::hasValidPath(const Vec2i& start, const Vec2i& goal) const {
    // Simple BFS pathfinding
    std::queue<Vec2i> queue;
    std::vector<std::vector<bool>> visited(m_height, std::vector<bool>(m_width, false));

    queue.push(start);
    visited[start.y][start.x] = true;

    Vec2i directions[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    while (!queue.empty()) {
        Vec2i current = queue.front();
        queue.pop();

        if (current == goal) {
            return true;
        }

        for (const auto& dir : directions) {
            Vec2i next = current + dir;

            if (isValidPosition(next) && !visited[next.y][next.x] && isWalkable(next)) {
                visited[next.y][next.x] = true;
                queue.push(next);
            }
        }
    }

    return false;
}

std::vector<Vec2i> Map::findPath(const Vec2i& start, const Vec2i& goal) const {
    // TODO: Implement actual pathfinding algorithm
    (void)start; (void)goal; // Suppress unused parameter warnings
    return std::vector<Vec2i>();
}

Vec2i Map::getRandomWalkablePosition(RNG& rng) const {
    Vec2i pos;
    int attempts = 0;
    do {
        pos.x = rng.rollRange(0, m_width - 1);
        pos.y = rng.rollRange(0, m_height - 1);
        attempts++;
    } while (!isWalkable(pos) && attempts < 100);

    return pos;
}

void Map::draw(sf::RenderTarget& target, const sf::View& view) const {
    // Get view bounds for culling
    sf::FloatRect viewBounds(
        view.getCenter().x - view.getSize().x / 2.0f,
        view.getCenter().y - view.getSize().y / 2.0f,
        view.getSize().x,
        view.getSize().y
    );

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            sf::Vector2f tilePos(x * Constants::TILE_SIZE, y * Constants::TILE_SIZE);

            // Simple culling
            if (tilePos.x + Constants::TILE_SIZE < viewBounds.left ||
                tilePos.x > viewBounds.left + viewBounds.width ||
                tilePos.y + Constants::TILE_SIZE < viewBounds.top ||
                tilePos.y > viewBounds.top + viewBounds.height) {
                continue;
            }

            m_tileShape.setPosition(tilePos);
            m_tileShape.setFillColor(getTileColor(m_tiles[y][x].getType()));
            target.draw(m_tileShape);
        }
    }
}

void Map::initializeTiles() {
    m_tiles.resize(m_height);
    for (int y = 0; y < m_height; ++y) {
        m_tiles[y].resize(m_width);
        for (int x = 0; x < m_width; ++x) {
            m_tiles[y][x].setType(TileType::Empty);
        }
    }
}

void Map::placeMandatoryTiles() {
    // Place start and goal
    setTileType(m_startPos, TileType::Empty);
    setTileType(m_goalPos, TileType::Boss);
}

void Map::distributeTiles(RNG& rng) {
    // Legacy method - simple random distribution for old tile types
    // This is only used if generateMap() is called instead of generateMaze()
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Vec2i pos(x, y);

            // Skip start and goal
            if (pos == m_startPos || pos == m_goalPos) {
                continue;
            }

            int roll = rng.rollRange(1, 100);

            if (roll <= Constants::TILE_ENEMY_PERCENT) {
                setTileType(pos, TileType::Enemy);
            } else if (roll <= Constants::TILE_ENEMY_PERCENT + Constants::TILE_ROCK_PERCENT) {
                setTileType(pos, TileType::Rock);
            }
            // Else remains Empty
        }
    }
}

void Map::ensurePathExists(RNG& rng) {
    // TODO: Use RNG for more sophisticated path generation
    (void)rng; // Suppress unused parameter warning

    // Simple path creation - clear a direct path if no path exists
    // This is a simplified implementation
    Vec2i current = m_startPos;
    Vec2i goal = m_goalPos;

    while (current != goal) {
        setTileType(current, TileType::Empty);

        if (current.x < goal.x) current.x++;
        else if (current.x > goal.x) current.x--;
        else if (current.y < goal.y) current.y++;
        else if (current.y > goal.y) current.y--;
    }
}

sf::Color Map::getTileColor(TileType type) const {
    switch (type) {
        case TileType::Empty: return sf::Color(50, 50, 50);        // Dark gray path
        case TileType::Wall: return sf::Color::Black;              // Black walls with white border
        case TileType::Rock: return sf::Color(139, 69, 19);        // Brown rocks
        case TileType::TeleportGate: return sf::Color(0, 255, 255); // Cyan teleport gates
        case TileType::Enemy: return sf::Color(255, 0, 0);         // Red enemies
        case TileType::Boss: return sf::Color(255, 0, 255);        // Magenta boss
        case TileType::Goal: return sf::Color(0, 255, 0);          // Green goal
        default: return sf::Color::White;
    }
}

char Map::getTileChar(TileType type) const {
    switch (type) {
        case TileType::Empty: return '.';
        case TileType::Wall: return 'W';
        case TileType::Rock: return '#';
        case TileType::TeleportGate: return 'T';
        case TileType::Enemy: return 'E';
        case TileType::Boss: return 'B';
        case TileType::Goal: return 'G';
        default: return '?';
    }
}

TileType Map::charToTileType(char c) const {
    switch (c) {
        case '.': return TileType::Empty;
        case '#': return TileType::Rock;
        case 'W': return TileType::Wall;
        case 'T': return TileType::TeleportGate;
        case 'G': return TileType::Goal;
        case 'E': return TileType::Enemy;
        case 'B': return TileType::Boss;
        default: return TileType::Empty;
    }
}

// Enhanced maze generation methods
void Map::generateMazeWalls(RNG& rng) {
    (void)rng; // rng not needed here
    // Fill entire map with walls first (black background is handled by clear())
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            setTileType(x, y, TileType::Wall);
        }
    }
}

static inline int manhattan(const Vec2i& a, const Vec2i& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

static inline bool insideInterior(const Vec2i& p, int w, int h) {
    return p.x > 0 && p.x < w - 1 && p.y > 0 && p.y < h - 1;
}

void Map::createMazePath(RNG& rng) {
    // Depth-first backtracker on grid with bias towards goal
    std::vector<std::vector<bool>> visited(m_height, std::vector<bool>(m_width, false));
    std::vector<Vec2i> stack;

    auto canCarve = [&](const Vec2i& next) {
        if (!insideInterior(next, m_width, m_height) || visited[next.y][next.x]) return false;
        // Avoid over-wide openings
        int adjEmpty = 0;
        Vec2i dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (auto& d : dirs) {
            Vec2i n = { next.x + d.x, next.y + d.y };
            if (isValidPosition(n) && getTileType(n) == TileType::Empty) adjEmpty++;
        }
        return adjEmpty <= 1;
    };

    Vec2i start = m_startPos;
    Vec2i goal  = m_goalPos;

    stack.push_back(start);
    visited[start.y][start.x] = true;
    setTileType(start, TileType::Empty);

    while (!stack.empty()) {
        Vec2i current = stack.back();
        if (current == goal) break;

        // Score neighbors by distance to goal
        std::vector<std::pair<int,Vec2i>> scored;
        Vec2i dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        for (auto& d : dirs) {
            Vec2i n = { current.x + d.x, current.y + d.y };
            if (canCarve(n)) {
                int score = manhattan(n, goal);
                scored.push_back({score, n});
            }
        }
        if (!scored.empty()) {
            // Shuffle then sort
            for (size_t i = 0; i < scored.size(); ++i) {
                size_t j = static_cast<size_t>(rng.rollRange(0, static_cast<int>(scored.size()) - 1));
                std::swap(scored[i], scored[j]);
            }
            std::sort(scored.begin(), scored.end(), [](auto& a, auto& b){ return a.first < b.first; });
            Vec2i next = scored[0].second;
            if (scored.size() > 1 && rng.rollRange(1, 100) <= 35) next = scored[1].second;
            visited[next.y][next.x] = true;
            setTileType(next, TileType::Empty);
            stack.push_back(next);
        } else {
            stack.pop_back();
        }
    }

    // Ensure connectivity to goal if not reached
    if (getTileType(goal) == TileType::Wall) {
        Vec2i cur = stack.empty() ? start : stack.back();
        while (cur.x != goal.x) { cur.x += (goal.x > cur.x ? 1 : -1); setTileType(cur, TileType::Empty); }
        while (cur.y != goal.y) { cur.y += (goal.y > cur.y ? 1 : -1); setTileType(cur, TileType::Empty); }
    }

    // Organic branches for aesthetics
    std::vector<Vec2i> corridorCells;
    for (int y = 1; y < m_height-1; ++y) for (int x = 1; x < m_width-1; ++x)
        if (getTileType(x,y) == TileType::Empty) corridorCells.push_back({x,y});

    int branches = 18;
    for (int b = 0; b < branches && !corridorCells.empty(); ++b) {
        Vec2i base = corridorCells[rng.rollRange(0, static_cast<int>(corridorCells.size()) - 1)];
        int len = 2 + rng.rollRange(0, 4);
        Vec2i dirs2[4] = { {1,0},{-1,0},{0,1},{0,-1} };
        Vec2i cur = base;
        for (int i = 0; i < len; ++i) {
            Vec2i d = dirs2[rng.rollRange(0,3)];
            Vec2i n = { cur.x + d.x, cur.y + d.y };
            if (canCarve(n)) { setTileType(n, TileType::Empty); cur = n; } else break;
        }
    }
}

void Map::createRandomBranch(RNG& rng, Vec2i start, int length) {
    Vec2i current = start;
    std::vector<Vec2i> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    for (int i = 0; i < length; ++i) {
        Vec2i dir = directions[rng.rollRange(0, 3)];
        Vec2i next = Vec2i(current.x + dir.x, current.y + dir.y);

        if (isValidPosition(next.x, next.y) &&
            next.x > 1 && next.x < m_width - 2 &&
            next.y > 1 && next.y < m_height - 2) {
            setTileType(next.x, next.y, TileType::Empty);
            current = next;
        } else {
            break; // Hit boundary or invalid position
        }
    }
}

void Map::placeMonsters(RNG& rng) {
    m_monsterPositions.clear();

    // Find empty positions on the path
    std::vector<Vec2i> emptyPositions;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Vec2i pos(x, y);
            if (getTileType(pos) == TileType::Empty && pos != m_startPos && pos != m_goalPos) {
                emptyPositions.push_back(pos);
            }
        }
    }

    // Place 2 normal monsters
    for (int i = 0; i < Constants::NORMAL_MONSTERS && !emptyPositions.empty(); ++i) {
        int index = rng.rollRange(0, emptyPositions.size() - 1);
        Vec2i monsterPos = emptyPositions[index];
        setTileType(monsterPos, TileType::Enemy);
        m_monsterPositions.push_back(monsterPos);
        emptyPositions.erase(emptyPositions.begin() + index);
    }

    // Boss is already placed at goal in placeMandatoryTiles()
    m_monsterPositions.push_back(m_goalPos);
}

void Map::placeRocks(RNG& rng) {
    m_rocks.clear();

    // Find empty positions on the path
    std::vector<Vec2i> emptyPositions;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Vec2i pos(x, y);
            if (getTileType(pos) == TileType::Empty && pos != m_startPos && pos != m_goalPos) {
                emptyPositions.push_back(pos);
            }
        }
    }

    // Place up to MAX_ROCKS_ON_PATH rocks
    int rocksToPlace = std::min(Constants::MAX_ROCKS_ON_PATH, static_cast<int>(emptyPositions.size()));
    for (int i = 0; i < rocksToPlace; ++i) {
        int index = rng.rollRange(0, emptyPositions.size() - 1);
        Vec2i rockPos = emptyPositions[index];
        setTileType(rockPos, TileType::Rock);
        m_rocks.emplace_back(rockPos);
        emptyPositions.erase(emptyPositions.begin() + index);
    }
}

void Map::setupTeleportGates(RNG& rng) {
    m_teleportGates.clear();

    // Find empty positions on the path
    std::vector<Vec2i> emptyPositions;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Vec2i pos(x, y);
            if (getTileType(pos) == TileType::Empty && pos != m_startPos && pos != m_goalPos) {
                emptyPositions.push_back(pos);
            }
        }
    }

    // Place teleport gate pairs
    for (int pairId = 0; pairId < Constants::TELEPORT_GATE_PAIRS && emptyPositions.size() >= 2; ++pairId) {
        // First gate
        int index1 = rng.rollRange(0, emptyPositions.size() - 1);
        Vec2i gate1Pos = emptyPositions[index1];
        emptyPositions.erase(emptyPositions.begin() + index1);

        // Second gate
        int index2 = rng.rollRange(0, emptyPositions.size() - 1);
        Vec2i gate2Pos = emptyPositions[index2];
        emptyPositions.erase(emptyPositions.begin() + index2);

        // Create paired gates
        TeleportGate gate1(gate1Pos, pairId);
        gate1.targetPosition = gate2Pos;
        TeleportGate gate2(gate2Pos, pairId);
        gate2.targetPosition = gate1Pos;

        m_teleportGates.push_back(gate1);
        m_teleportGates.push_back(gate2);

        setTileType(gate1Pos, TileType::TeleportGate);
        setTileType(gate2Pos, TileType::TeleportGate);
    }
}

// Enhanced feature methods
bool Map::canBreakRock(const Vec2i& pos) const {
    const RockState* rock = getRockState(pos);
    return rock && !rock->isBroken;
}

void Map::breakRock(const Vec2i& pos) {
    RockState* rock = getRockState(pos);
    if (rock && !rock->isBroken) {
        rock->breakProgress++;
        if (rock->breakProgress >= Constants::ROCK_BREAK_TURNS) {
            rock->isBroken = true;
            setTileType(pos, TileType::Empty);
        }
    }
}

Vec2i Map::getTeleportDestination(const Vec2i& gatePos) const {
    const TeleportGate* gate = getTeleportGate(gatePos);
    if (gate) {
        // Return position after the target gate (next cell in path direction)
        Vec2i target = gate->targetPosition;
        // For simplicity, return the target position + 1 in X direction
        Vec2i destination = Vec2i(target.x + 1, target.y);
        if (isValidPosition(destination) && isWalkable(destination)) {
            return destination;
        }
        return target; // Fallback to gate position
    }
    return gatePos; // No teleport
}

// Rock management
void Map::addRock(const Vec2i& pos) {
    m_rocks.emplace_back(pos);
    setTileType(pos, TileType::Rock);
}

RockState* Map::getRockState(const Vec2i& pos) {
    for (auto& rock : m_rocks) {
        if (rock.position == pos) {
            return &rock;
        }
    }
    return nullptr;
}

const RockState* Map::getRockState(const Vec2i& pos) const {
    for (const auto& rock : m_rocks) {
        if (rock.position == pos) {
            return &rock;
        }
    }
    return nullptr;
}

// Teleport gate management
void Map::addTeleportGate(const Vec2i& pos, int pairId) {
    TeleportGate gate(pos, pairId);
    m_teleportGates.push_back(gate);
    setTileType(pos, TileType::TeleportGate);
}

const TeleportGate* Map::getTeleportGate(const Vec2i& pos) const {
    for (const auto& gate : m_teleportGates) {
        if (gate.position == pos) {
            return &gate;
        }
    }
    return nullptr;
}
