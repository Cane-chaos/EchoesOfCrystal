#include "entities/Enemy.h"
#include "Constants.h"

Enemy::Enemy(const std::string& name, PokemonType type)
    : m_name(name)
{
    setType(type);
    m_stats = Stats(Constants::BASE_ENEMY_HP, Constants::BASE_ENEMY_MP, 
                   Constants::BASE_ENEMY_ATK, Constants::BASE_ENEMY_DEF);
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    
    m_sprite.setSize(sf::Vector2f(Constants::TILE_SIZE * 0.6f, Constants::TILE_SIZE * 0.6f));
    m_sprite.setOrigin(m_sprite.getSize().x / 2.0f, m_sprite.getSize().y / 2.0f);
    m_sprite.setFillColor(sf::Color::Red);
    
    setupSkills();
}

Enemy::~Enemy() {
}

CombatAction Enemy::chooseAction(const Entity& target) const {
    (void)target; // Unused in simple AI for now
    // Simple AI: use skill if available and has MP, otherwise normal attack
    for (const auto& skill : m_skills) {
        if (skill->canUse(m_currentMP)) {
            return CombatAction::UseSkill;
        }
    }
    return CombatAction::NormalAttack;
}

void Enemy::performAction(CombatAction action, Entity& target) {
    switch (action) {
        case CombatAction::UseSkill:
            // Use first available skill
            for (auto& skill : m_skills) {
                if (skill->canUse(m_currentMP)) {
                    useSkill(skill->getType(), target);
                    break;
                }
            }
            break;
        case CombatAction::NormalAttack:
            normalAttack(target);
            break;
        default:
            break;
    }
}

void Enemy::addSkill(std::unique_ptr<Skill> skill) {
    m_skills.push_back(std::move(skill));
}

std::vector<Skill*> Enemy::getAvailableSkills() const {
    std::vector<Skill*> available;
    for (const auto& skill : m_skills) {
        if (skill->canUse(m_currentMP)) {
            available.push_back(skill.get());
        }
    }
    return available;
}

void Enemy::normalAttack(Entity& target) {
    int damage = m_stats.atk - target.getStats().def;
    damage = std::max(1, damage);
    target.modifyHP(-damage);
}

void Enemy::useSkill(SkillType skillType, Entity& target) {
    for (auto& skill : m_skills) {
        if (skill->getType() == skillType && skill->canUse(m_currentMP)) {
            modifyMP(-skill->getData().mpCost);
            skill->use();

            if (skill->getData().isOffensive) {
                int damage = skill->calculateDamage(m_stats.atk);
                target.modifyHP(-damage);
            }
            break;
        }
    }
}

void Enemy::updateCooldowns() {
    for (auto& skill : m_skills) {
        skill->updateCooldown();
    }
}

std::unique_ptr<Enemy> Enemy::createRegularEnemy() {
    return std::make_unique<Enemy>("Wild Pokemon");
}

void Enemy::update(sf::Time dt) {
    (void)dt; // Not used yet
    // Update world position of the sprite here (not in draw)
    m_sprite.setPosition(m_position);
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Do not mutate state in const draw
    target.draw(m_sprite, states);
}

void Enemy::setupSkills() {
    // Give enemy a basic skill
    addSkill(Skill::createPowerStrike());
}
