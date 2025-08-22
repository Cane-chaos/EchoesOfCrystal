#include "world/Tile.h"

Tile::Tile()
    : m_type(TileType::Empty)
    , m_triggered(false)
{
}

Tile::Tile(TileType type)
    : m_type(type)
    , m_triggered(false)
{
}

bool Tile::isWalkable() const {
    // Walkable if not a blocking obstacle
    return m_type != TileType::Rock && m_type != TileType::Wall;
}

bool Tile::blocksMovement() const {
    return m_type == TileType::Rock || m_type == TileType::Wall;
}

bool Tile::hasEvent() const {
    switch (m_type) {
        case TileType::TeleportGate:
        case TileType::Enemy:
        case TileType::Boss:
        case TileType::Goal:
            return true;
        default:
            return false;
    }
}

Direction Tile::getArrowDirection() const {
    // Arrows removed in enhanced map; keep default
    return Direction::Up;
}
