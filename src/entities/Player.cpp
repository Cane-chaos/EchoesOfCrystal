#include "entities/Player.h"
#include "entities/Pokemon.h"
#include "world/Map.h"
#include "Constants.h"
#include <algorithm>

Player::Player()
    : m_mapPosition(Constants::START_X, Constants::START_Y)
    , m_direction(Direction::Up)
    , m_experience(0)
    , m_victories(0)
    , m_upgradePoints(0)
{
    m_sprite.setRadius(Constants::TILE_SIZE / 4.0f);
    m_sprite.setFillColor(sf::Color::Green);
    m_sprite.setOrigin(m_sprite.getRadius(), m_sprite.getRadius());
}

Player::~Player() {
}

void Player::setPokemon(std::unique_ptr<Pokemon> pokemon) {
    m_pokemon = std::move(pokemon);
    updateStatsFromPokemon();
}

void Player::setMapPosition(const Vec2i& position) {
    m_mapPosition = position;
}

Vec2i Player::calculateMovement(int steps) const {
    Vec2i dirVector = directionToVector(m_direction);
    return m_mapPosition + dirVector * steps;
}

void Player::moveSteps(int steps, const Map& map) {
    Vec2i target = calculateMovement(steps);
    
    // Handle bounds
    target.x = std::max(0, std::min(target.x, map.getWidth() - 1));
    target.y = std::max(0, std::min(target.y, map.getHeight() - 1));
    
    // Check for obstacles
    if (map.getTileType(target) == TileType::Rock) {
        // Stop before the rock
        Vec2i dirVector = directionToVector(m_direction);
        target = m_mapPosition + dirVector * (steps - 1);
        if (!map.isValidPosition(target)) {
            target = m_mapPosition;
        }
    }
    
    setMapPosition(target);
}

void Player::gainExperience(int amount) {
    m_experience += amount;
    // Simple level up check
    int requiredExp = m_level * 100;
    if (m_experience >= requiredExp) {
        levelUp();
    }
}

void Player::levelUp() {
    m_level++;
    
    // Increase stats
    m_stats.atk += Constants::ATK_PER_LEVEL;
    m_stats.def += Constants::DEF_PER_LEVEL;
    m_stats.hp += Constants::HP_PER_LEVEL;
    m_stats.mp += Constants::MP_PER_LEVEL;
    
    // Heal to full
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    
    if (m_pokemon) {
        m_pokemon->setStats(m_stats);
        m_pokemon->setCurrentHP(m_currentHP);
        m_pokemon->setCurrentMP(m_currentMP);
    }
}

bool Player::canEvolve() const {
    return m_level >= Constants::EVOLUTION_LEVEL && m_pokemon && !m_pokemon->isEvolved();
}

void Player::evolve() {
    if (canEvolve() && m_pokemon) {
        m_pokemon->evolve();
        updateStatsFromPokemon();
    }
}

void Player::addVictory() {
    m_victories++;
    gainExperience(50); // Fixed experience per victory
    
    // Add upgrade points
    if (m_victories % Constants::VICTORIES_FOR_UPGRADE == 0) {
        addUpgradePoints(Constants::UPGRADE_POINTS_PER_VICTORIES);
    }
}

void Player::spendUpgradePoints(int points) {
    m_upgradePoints = std::max(0, m_upgradePoints - points);
}

void Player::addUpgradePoints(int points) {
    m_upgradePoints += points;
}

void Player::upgradeAttack(int points) {
    if (m_upgradePoints >= points) {
        m_stats.atk += points;
        spendUpgradePoints(points);
        updateStatsFromPokemon();
    }
}

void Player::upgradeDefense(int points) {
    if (m_upgradePoints >= points) {
        m_stats.def += points;
        spendUpgradePoints(points);
        updateStatsFromPokemon();
    }
}

void Player::upgradeHealth(int points) {
    if (m_upgradePoints >= points) {
        m_stats.hp += points * 5; // 5 HP per point
        spendUpgradePoints(points);
        updateStatsFromPokemon();
    }
}

SaveData Player::createSaveData() const {
    SaveData data;
    data.mapSeed = 0; // Would be set by map
    data.position = m_mapPosition;
    data.direction = m_direction;
    data.level = m_level;
    data.baseStats = m_stats;
    data.currentHP = m_currentHP;
    data.currentMP = m_currentMP;
    data.victories = m_victories;
    data.rngSeed = 0; // Would be set by game
    data.pokemonType = m_type;
    data.upgradePoints = m_upgradePoints;
    
    return data;
}

void Player::loadFromSaveData(const SaveData& data) {
    m_mapPosition = data.position;
    m_direction = data.direction;
    m_level = data.level;
    m_stats = data.baseStats;
    m_currentHP = data.currentHP;
    m_currentMP = data.currentMP;
    m_victories = data.victories;
    m_type = data.pokemonType;
    m_upgradePoints = data.upgradePoints;
    
    updateStatsFromPokemon();
}

void Player::update(sf::Time dt) {
    if (m_pokemon) {
        m_pokemon->update(dt);
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Draw without modifying the member in a const method
    sf::CircleShape spriteCopy = m_sprite;
    spriteCopy.setPosition(m_position);
    target.draw(spriteCopy, states);
}

// Auto-path system implementation
void Player::initVisitedGlobal(int width, int height) {
    m_visitedGlobal.assign(height, std::vector<bool>(width, false));
}

void Player::markVisited(const Vec2i& pos) {
    if (pos.y >= 0 && pos.y < static_cast<int>(m_visitedGlobal.size()) &&
        pos.x >= 0 && pos.x < static_cast<int>(m_visitedGlobal[0].size())) {
        m_visitedGlobal[pos.y][pos.x] = true;
    }
}

bool Player::hasVisited(const Vec2i& pos) const {
    if (pos.y >= 0 && pos.y < static_cast<int>(m_visitedGlobal.size()) &&
        pos.x >= 0 && pos.x < static_cast<int>(m_visitedGlobal[0].size())) {
        return m_visitedGlobal[pos.y][pos.x];
    }
    return false;
}

void Player::resetVisitedGlobal() {
    for (auto& row : m_visitedGlobal) {
        std::fill(row.begin(), row.end(), false);
    }
}

void Player::updateStatsFromPokemon() {
    if (m_pokemon) {
        m_pokemon->setStats(m_stats);
        m_pokemon->setCurrentHP(m_currentHP);
        m_pokemon->setCurrentMP(m_currentMP);
        m_pokemon->setLevel(m_level);
    }
}
