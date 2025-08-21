#pragma once
#include <SFML/Graphics.hpp>
#include "Types.h"

class Entity {
public:
    Entity();
    virtual ~Entity();
    
    // Position
    void setPosition(const Vec2f& position);
    void setPosition(float x, float y);
    Vec2f getPosition() const;
    
    // Stats
    const Stats& getStats() const { return m_stats; }
    void setStats(const Stats& stats) { m_stats = stats; }
    
    // Health and Mana
    int getCurrentHP() const { return m_currentHP; }
    int getCurrentMP() const { return m_currentMP; }
    int getMaxHP() const { return m_stats.hp; }
    int getMaxMP() const { return m_stats.mp; }
    
    void setCurrentHP(int hp);
    void setCurrentMP(int mp);
    void modifyHP(int amount);
    void modifyMP(int amount);
    
    // Status
    bool isAlive() const { return m_currentHP > 0; }
    bool isDead() const { return m_currentHP <= 0; }
    
    // Level and experience
    int getLevel() const { return m_level; }
    void setLevel(int level) { m_level = level; }
    
    // Type
    PokemonType getType() const { return m_type; }
    void setType(PokemonType type) { m_type = type; }
    
    // Virtual methods
    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const = 0;
    
protected:
    Vec2f m_position;
    Stats m_stats;
    int m_currentHP;
    int m_currentMP;
    int m_level;
    PokemonType m_type;
};
