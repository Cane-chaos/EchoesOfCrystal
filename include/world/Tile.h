#pragma once
#include "Types.h"

class Tile {
public:
    Tile();
    explicit Tile(TileType type);
    
    // Type management
    TileType getType() const { return m_type; }
    void setType(TileType type) { m_type = type; }
    
    // Properties
    bool isWalkable() const;
    bool blocksMovement() const;
    bool hasEvent() const;
    
    // Event handling
    bool isTriggered() const { return m_triggered; }
    void setTriggered(bool triggered) { m_triggered = triggered; }
    
    // Arrow direction (for arrow tiles)
    Direction getArrowDirection() const;
    
private:
    TileType m_type;
    bool m_triggered;
};
