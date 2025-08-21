#include "entities/Boss.h"
#include "Constants.h"

Boss::Boss() : Enemy("Boss", PokemonType::Fire) {
    setupBossStats();
    m_sprite.setFillColor(sf::Color::Magenta);
    m_sprite.setSize(sf::Vector2f(Constants::TILE_SIZE, Constants::TILE_SIZE));
    m_sprite.setOrigin(m_sprite.getSize().x / 2.0f, m_sprite.getSize().y / 2.0f);
}

Boss::~Boss() {
}

CombatAction Boss::chooseAction(const Entity& target) const {
    // Boss AI: prioritize attack when target HP < 40%
    float targetHPPercent = static_cast<float>(target.getCurrentHP()) / target.getMaxHP();
    
    if (targetHPPercent < 0.4f) {
        // Aggressive mode - use strongest available skill
        for (const auto& skill : m_skills) {
            if (skill->canUse(m_currentMP) && skill->getData().damageMultiplier > 1.0f) {
                return CombatAction::UseSkill;
            }
        }
    }
    
    // Normal behavior
    return Enemy::chooseAction(target);
}

int Boss::modifyIncomingDamage(int damage) const {
    // Resolute passive: reduce damage by 10%
    return static_cast<int>(damage * Constants::BOSS_DAMAGE_REDUCTION);
}

std::unique_ptr<Boss> Boss::createBoss() {
    return std::make_unique<Boss>();
}

void Boss::setupSkills() {
    // Boss has multiple skills
    addSkill(Skill::createPowerStrike());
    addSkill(Skill::createCleave());
    addSkill(Skill::createGuard());
}

void Boss::setupBossStats() {
    m_stats = Stats(Constants::BASE_BOSS_HP, Constants::BASE_BOSS_MP,
                   Constants::BASE_BOSS_ATK, Constants::BASE_BOSS_DEF);
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
}
