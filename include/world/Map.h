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
    void generateMaze(RNG& rng, unsigned int seed = 0);  // New maze generation
    void generateZigZag(RNG& rng, unsigned int seed = 0); // Serpentine track (Monopoly-like)
    void generateMap(RNG& rng, unsigned int seed = 0);   // Legacy method
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
    
    // Enhanced features
    bool canBreakRock(const Vec2i& pos) const;
    void breakRock(const Vec2i& pos);
    Vec2i getTeleportDestination(const Vec2i& gatePos) const;
    void setupTeleportGates(RNG& rng);

    // Rock management
    void addRock(const Vec2i& pos);
    RockState* getRockState(const Vec2i& pos);
    const RockState* getRockState(const Vec2i& pos) const;

    // Teleport gate management
    void addTeleportGate(const Vec2i& pos, int pairId);
    const TeleportGate* getTeleportGate(const Vec2i& pos) const;

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
    // Map generation methods
    void initializeTiles();
    void generateMazeWalls(RNG& rng);
    void createMazePath(RNG& rng);
    void createRandomBranch(RNG& rng, Vec2i start, int length);
    void createZigZagPath();
    void placeMandatoryTiles();
    void placeMonsters(RNG& rng);
    void placeRocks(RNG& rng);
    void distributeTiles(RNG& rng);  // Legacy method
    void ensurePathExists(RNG& rng);

    // Utility methods
    sf::Color getTileColor(TileType type) const;
    char getTileChar(TileType type) const;
    TileType charToTileType(char c) const;
    bool isValidMazePosition(const Vec2i& pos) const;

private:
    int m_width;
    int m_height;
    std::vector<std::vector<Tile>> m_tiles;

    Vec2i m_startPos;
    Vec2i m_goalPos;

    // Enhanced features
    std::vector<RockState> m_rocks;
    std::vector<TeleportGate> m_teleportGates;
    std::vector<Vec2i> m_monsterPositions;

    // Rendering
    mutable sf::RectangleShape m_tileShape;
};
