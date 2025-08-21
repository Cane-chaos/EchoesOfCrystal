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
    return m_type != TileType::Rock;
}

bool Tile::blocksMovement() const {
    return m_type == TileType::Rock;
}

bool Tile::hasEvent() const {
    switch (m_type) {
        case TileType::Pit:
        case TileType::Rift:
        case TileType::ArrowUp:
        case TileType::ArrowDown:
        case TileType::ArrowLeft:
        case TileType::ArrowRight:
        case TileType::Enemy:
        case TileType::Boss:
            return true;
        default:
            return false;
    }
}

Direction Tile::getArrowDirection() const {
    switch (m_type) {
        case TileType::ArrowUp: return Direction::Up;
        case TileType::ArrowDown: return Direction::Down;
        case TileType::ArrowLeft: return Direction::Left;
        case TileType::ArrowRight: return Direction::Right;
        default: return Direction::Up; // Default fallback
    }
}
