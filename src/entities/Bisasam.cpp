#include "entities/Bisasam.h"
#include "entities/Skill.h"
#include "core/RNG.h"
#include "Constants.h"
#include <memory>
#include <vector>

Bisasam::Bisasam() : Enemy("Bisasam") {
    setupBisasam();
}

Bisasam::~Bisasam() {
}

CombatAction Bisasam::chooseAction(const Entity& target) const {
    (void)target; // Simple AI doesn't analyze target
    
    // 70% chance to use special skill if available, 30% normal attack
    static RNG rng; // Simple static RNG for AI decisions
    
    if (rng.rollRange(1, 100) <= 70) {
        // Try to use special skill
        for (const auto& skill : m_skills) {
            if (skill->canUse(m_currentMP) && skill->getType() != SkillType::Tackle) {
                return CombatAction::UseSkill;
            }
        }
    }
    
    return CombatAction::NormalAttack;
}

SkillType Bisasam::selectRandomSpecialSkill() const {
    std::vector<SkillType> availableSkills;
    
    for (const auto& skill : m_skills) {
        if (skill->canUse(m_currentMP) && skill->getType() != SkillType::Tackle) {
            availableSkills.push_back(skill->getType());
        }
    }
    
    if (!availableSkills.empty()) {
        static RNG rng;
        int index = rng.rollRange(0, availableSkills.size() - 1);
        return availableSkills[index];
    }
    
    return SkillType::Tackle; // Fallback to normal attack
}

bool Bisasam::attemptDodge() const {
    static RNG rng;
    return rng.rollRange(1, 100) <= static_cast<int>(Constants::MONSTER_DODGE_CHANCE * 100);
}

std::unique_ptr<Bisasam> Bisasam::create() {
    return std::make_unique<Bisasam>();
}

void Bisasam::setupSkills() {
    m_skills.clear();
    
    // Bisasam has 1 normal attack + 1 special skill
    addSkill(Skill::createTackle());    // Normal attack
    addSkill(Skill::createVineWhip());  // Special grass attack
}

void Bisasam::setupBisasam() {
    // Set Bisasam stats (slightly different from Chalamander)
    m_stats = Stats(
        Constants::BASE_ENEMY_HP + 5,  // HP: 75 (slightly more HP)
        Constants::BASE_ENEMY_MP - 5,  // MP: 25 (slightly less MP)
        Constants::BASE_ENEMY_ATK - 2, // ATK: 13 (slightly less ATK)
        Constants::BASE_ENEMY_DEF + 2  // DEF: 10 (slightly more DEF)
    );
    
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    m_level = 1;
    
    setupSkills();
    
    // Setup sprite (placeholder - would load from assets/icon/bisasam.png)
    m_sprite.setRadius(16.0f);
    m_sprite.setFillColor(sf::Color::Green);
    m_sprite.setOrigin(16.0f, 16.0f);
}
