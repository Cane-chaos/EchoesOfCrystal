#include "entities/Skill.h"
#include "entities/Entity.h"
#include "Constants.h"
#include <algorithm>

Skill::Skill(SkillType type, const std::string& name, const SkillData& data)
    : m_type(type)
    , m_name(name)
    , m_data(data)
    , m_currentCooldown(0)
{
}

Skill::~Skill() {
}

bool Skill::canUse(int currentMP) const {
    return currentMP >= m_data.mpCost && m_currentCooldown == 0;
}

void Skill::use() {
    m_currentCooldown = m_data.cooldown;
}

void Skill::updateCooldown() {
    if (m_currentCooldown > 0) {
        m_currentCooldown--;
    }
}

void Skill::resetCooldown() {
    m_currentCooldown = 0;
}

int Skill::calculateDamage(int attackerATK) const {
    return static_cast<int>(attackerATK * m_data.damageMultiplier);
}

int Skill::calculateHealing(int attackerATK) const {
    return static_cast<int>(attackerATK * m_data.healMultiplier);
}

void Skill::applySpecialEffect(Entity& user, Entity& target) const {
    (void)user; (void)target; // TODO: implement special effects
    // Implement special effects based on skill type
    switch (m_type) {
        case SkillType::Guard:
            // Guard effect would be handled in combat system
            break;
        case SkillType::Counter:
            // Counter effect would be handled in combat system
            break;
        case SkillType::Fireball:
        case SkillType::AquaPulse:
        case SkillType::LeafBlade:
            // 20% chance to apply debuff (simplified)
            break;
        default:
            break;
    }
}

std::unique_ptr<Skill> Skill::createPowerStrike() {
    SkillData data(SkillType::PowerStrike, 8, 1, 1.6f, 0.0f, true, false, false);
    return std::make_unique<Skill>(SkillType::PowerStrike, "Power Strike", data);
}

std::unique_ptr<Skill> Skill::createCleave() {
    SkillData data(SkillType::Cleave, 12, 2, 1.2f, 0.0f, true, false, false);
    return std::make_unique<Skill>(SkillType::Cleave, "Cleave", data);
}

std::unique_ptr<Skill> Skill::createHeal() {
    SkillData data(SkillType::Heal, 10, 2, 0.0f, 0.8f, false, false, false);
    return std::make_unique<Skill>(SkillType::Heal, "Heal", data);
}

std::unique_ptr<Skill> Skill::createGuard() {
    SkillData data(SkillType::Guard, 6, 2, 0.0f, 0.0f, false, true, true);
    return std::make_unique<Skill>(SkillType::Guard, "Guard", data);
}

std::unique_ptr<Skill> Skill::createCounter() {
    SkillData data(SkillType::Counter, 10, 3, 0.0f, 0.0f, false, true, true);
    return std::make_unique<Skill>(SkillType::Counter, "Counter", data);
}

std::unique_ptr<Skill> Skill::createFireball() {
    SkillData data(SkillType::Fireball, 12, 2, 1.4f, 0.0f, true, false, true);
    return std::make_unique<Skill>(SkillType::Fireball, "Fireball", data);
}

std::unique_ptr<Skill> Skill::createAquaPulse() {
    SkillData data(SkillType::AquaPulse, 12, 2, 1.4f, 0.0f, true, false, true);
    return std::make_unique<Skill>(SkillType::AquaPulse, "Aqua Pulse", data);
}

std::unique_ptr<Skill> Skill::createLeafBlade() {
    SkillData data(SkillType::LeafBlade, 12, 2, 1.4f, 0.0f, true, false, true);
    return std::make_unique<Skill>(SkillType::LeafBlade, "Leaf Blade", data);
}
