#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Tile.h"
#include "Types.h"

class RNG;

class Map {
public:
    Map();
    ~Map();
    
    // Initialization
    void generateMap(RNG& rng, unsigned int seed = 0);
    bool loadFromCSV(const std::string& filename);
    bool saveToCSV(const std::string& filename) const;
    
    // Tile access
    TileType getTileType(int x, int y) const;
    TileType getTileType(const Vec2i& pos) const;
    void setTileType(int x, int y, TileType type);
    void setTileType(const Vec2i& pos, TileType type);
    
    // Bounds checking
    bool isValidPosition(int x, int y) const;
    bool isValidPosition(const Vec2i& pos) const;
    bool isWalkable(int x, int y) const;
    bool isWalkable(const Vec2i& pos) const;
    
    // Pathfinding
    bool hasValidPath(const Vec2i& start, const Vec2i& goal) const;
    std::vector<Vec2i> findPath(const Vec2i& start, const Vec2i& goal) const;
    
    // Random position generation
    Vec2i getRandomWalkablePosition(RNG& rng) const;
    
    // Rendering
    void draw(sf::RenderTarget& target, const sf::View& view) const;
    
    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    Vec2i getStartPosition() const { return m_startPos; }
    Vec2i getGoalPosition() const { return m_goalPos; }
    
private:
    void initializeTiles();
    void placeMandatoryTiles();
    void distributeTiles(RNG& rng);
    void ensurePathExists(RNG& rng);
    
    sf::Color getTileColor(TileType type) const;
    char getTileChar(TileType type) const;
    TileType charToTileType(char c) const;
    
private:
    int m_width;
    int m_height;
    std::vector<std::vector<Tile>> m_tiles;
    
    Vec2i m_startPos;
    Vec2i m_goalPos;
    
    // Rendering
    mutable sf::RectangleShape m_tileShape;
};
