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

void Map::generateMap(RNG& rng, unsigned int seed) {
    if (seed != 0) {
        rng.setSeed(seed);
    }
    
    initializeTiles();
    placeMandatoryTiles();
    distributeTiles(rng);
    
    // Ensure path exists
    if (!hasValidPath(m_startPos, m_goalPos)) {
        ensurePathExists(rng);
    }
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
    // Simple random distribution
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
            } else if (roll <= Constants::TILE_ENEMY_PERCENT + Constants::TILE_PIT_PERCENT) {
                setTileType(pos, TileType::Pit);
            } else if (roll <= Constants::TILE_ENEMY_PERCENT + Constants::TILE_PIT_PERCENT + Constants::TILE_RIFT_PERCENT) {
                setTileType(pos, TileType::Rift);
            } else if (roll <= Constants::TILE_ENEMY_PERCENT + Constants::TILE_PIT_PERCENT + Constants::TILE_RIFT_PERCENT + Constants::TILE_ROCK_PERCENT) {
                setTileType(pos, TileType::Rock);
            } else if (roll <= Constants::TILE_ENEMY_PERCENT + Constants::TILE_PIT_PERCENT + Constants::TILE_RIFT_PERCENT + Constants::TILE_ROCK_PERCENT + Constants::TILE_ARROW_PERCENT) {
                // Random arrow direction
                TileType arrows[] = {TileType::ArrowUp, TileType::ArrowDown, TileType::ArrowLeft, TileType::ArrowRight};
                setTileType(pos, arrows[rng.rollRange(0, 3)]);
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
        case TileType::Empty: return sf::Color(100, 100, 100);
        case TileType::Rock: return sf::Color(80, 80, 80);
        case TileType::Pit: return sf::Color(139, 69, 19);
        case TileType::Rift: return sf::Color(128, 0, 128);
        case TileType::ArrowUp:
        case TileType::ArrowDown:
        case TileType::ArrowLeft:
        case TileType::ArrowRight: return sf::Color(255, 255, 0);
        case TileType::Enemy: return sf::Color(255, 0, 0);
        case TileType::Boss: return sf::Color(255, 0, 255);
        default: return sf::Color::White;
    }
}

char Map::getTileChar(TileType type) const {
    switch (type) {
        case TileType::Empty: return '.';
        case TileType::Rock: return '#';
        case TileType::Pit: return 'P';
        case TileType::Rift: return 'R';
        case TileType::ArrowUp: return '^';
        case TileType::ArrowDown: return 'v';
        case TileType::ArrowLeft: return '<';
        case TileType::ArrowRight: return '>';
        case TileType::Enemy: return 'E';
        case TileType::Boss: return 'B';
        default: return '?';
    }
}

TileType Map::charToTileType(char c) const {
    switch (c) {
        case '.': return TileType::Empty;
        case '#': return TileType::Rock;
        case 'P': return TileType::Pit;
        case 'R': return TileType::Rift;
        case '^': return TileType::ArrowUp;
        case 'v': return TileType::ArrowDown;
        case '<': return TileType::ArrowLeft;
        case '>': return TileType::ArrowRight;
        case 'E': return TileType::Enemy;
        case 'B': return TileType::Boss;
        default: return TileType::Empty;
    }
}
