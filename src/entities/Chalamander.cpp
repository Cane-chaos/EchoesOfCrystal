#include "entities/Chalamander.h"
#include "entities/Skill.h"
#include "core/RNG.h"
#include "Constants.h"
#include <memory>
#include <vector>

Chalamander::Chalamander() : Enemy("Chalamander") {
    setupChalamander();
}

Chalamander::~Chalamander() {
}

CombatAction Chalamander::chooseAction(const Entity& target) const {
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

SkillType Chalamander::selectRandomSpecialSkill() const {
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

bool Chalamander::attemptDodge() const {
    static RNG rng;
    return rng.rollRange(1, 100) <= static_cast<int>(Constants::MONSTER_DODGE_CHANCE * 100);
}

std::unique_ptr<Chalamander> Chalamander::create() {
    return std::make_unique<Chalamander>();
}

void Chalamander::setupSkills() {
    m_skills.clear();
    
    // Chalamander has 1 normal attack + 1 special skill
    addSkill(Skill::createTackle());  // Normal attack
    addSkill(Skill::createEmber());   // Special fire attack
}

void Chalamander::setupChalamander() {
    // Set Chalamander stats
    m_stats = Stats(
        Constants::BASE_ENEMY_HP,      // HP: 70
        Constants::BASE_ENEMY_MP,      // MP: 30
        Constants::BASE_ENEMY_ATK,     // ATK: 15
        Constants::BASE_ENEMY_DEF      // DEF: 8
    );
    
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    m_level = 1;
    
    setupSkills();
    
    // Setup sprite (placeholder - would load from assets/icon/chalamander.png)
    m_sprite.setRadius(16.0f);
    m_sprite.setFillColor(sf::Color::Red);
    m_sprite.setOrigin(16.0f, 16.0f);
}
