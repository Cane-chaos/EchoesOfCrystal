#pragma once
#include "Entity.h"
#include "Pokemon.h"
#include <memory>

class Player : public Entity {
public:
    Player();
    ~Player();
    
    // Pokemon management
    void setPokemon(std::unique_ptr<Pokemon> pokemon);
    Pokemon* getPokemon() const { return m_pokemon.get(); }
    
    // Map position
    void setMapPosition(const Vec2i& position);
    Vec2i getMapPosition() const { return m_mapPosition; }
    
    // Direction
    void setDirection(Direction direction) { m_direction = direction; }
    Direction getDirection() const { return m_direction; }
    
    // Movement
    Vec2i calculateMovement(int steps) const;
    void moveSteps(int steps, const class Map& map);
    
    // Progression
    void gainExperience(int amount);
    void levelUp();
    bool canEvolve() const;
    void evolve();
    
    // Victories and upgrade points
    int getVictories() const { return m_victories; }
    void addVictory();
    int getUpgradePoints() const { return m_upgradePoints; }
    void spendUpgradePoints(int points);
    void addUpgradePoints(int points);
    
    // Stat upgrades
    void upgradeAttack(int points);
    void upgradeDefense(int points);
    void upgradeHealth(int points);
    
    // Save/Load
    SaveData createSaveData() const;
    void loadFromSaveData(const SaveData& data);
    
    // Entity overrides
    virtual void update(sf::Time dt) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
private:
    void updateStatsFromPokemon();
    
private:
    std::unique_ptr<Pokemon> m_pokemon;
    Vec2i m_mapPosition;
    Direction m_direction;
    
    int m_experience;
    int m_victories;
    int m_upgradePoints;
    
    // Visual representation
    sf::CircleShape m_sprite;
};
