#include "entities/Entity.h"
#include "Constants.h"
#include <algorithm>

Entity::Entity()
    : m_position(0, 0)
    , m_stats()
    , m_currentHP(0)
    , m_currentMP(0)
    , m_level(1)
    , m_type(PokemonType::Fire)
{
}

Entity::~Entity() {
}

void Entity::setPosition(const Vec2f& position) {
    m_position = position;
}

void Entity::setPosition(float x, float y) {
    m_position = Vec2f(x, y);
}

Vec2f Entity::getPosition() const {
    return m_position;
}

void Entity::setCurrentHP(int hp) {
    m_currentHP = std::max(0, std::min(hp, m_stats.hp));
}

void Entity::setCurrentMP(int mp) {
    m_currentMP = std::max(0, std::min(mp, m_stats.mp));
}

void Entity::modifyHP(int amount) {
    setCurrentHP(m_currentHP + amount);
}

void Entity::modifyMP(int amount) {
    setCurrentMP(m_currentMP + amount);
}
