#include "entities/Pikachu.h"
#include "entities/Skill.h"
#include "Constants.h"
#include <memory>

Pikachu::Pikachu() : Pokemon(PokemonType::Fire) { // Pikachu as Electric type (using Fire slot)
    setupPikachuSkills();
}

Pikachu::~Pikachu() {
}

std::vector<SkillType> Pikachu::getAttackSkills() const {
    return {SkillType::ThunderBolt, SkillType::QuickAttack, SkillType::IronTail};
}

std::vector<SkillType> Pikachu::getDefenseSkills() const {
    return {SkillType::Agility, SkillType::DoubleTeam, SkillType::Substitute};
}

std::unique_ptr<Pikachu> Pikachu::create() {
    return std::make_unique<Pikachu>();
}

void Pikachu::setupBaseSkills() {
    // Clear existing skills
    m_skills.clear();
    
    // Add Pikachu's 6 special skills
    // Attack skills
    m_skills.push_back(Skill::createThunderBolt());
    m_skills.push_back(Skill::createQuickAttack());
    m_skills.push_back(Skill::createIronTail());
    
    // Defense skills
    m_skills.push_back(Skill::createAgility());
    m_skills.push_back(Skill::createDoubleTeam());
    m_skills.push_back(Skill::createSubstitute());
}

void Pikachu::setupEvolutionSkill() {
    // Pikachu doesn't evolve in this game - keep same skills
    // Could add enhanced versions if needed
}

Stats Pikachu::getBaseStats() const {
    return Stats(
        Constants::BASE_PLAYER_HP,      // HP: 100
        Constants::BASE_PLAYER_MP,      // MP: 50
        Constants::BASE_PLAYER_ATK,     // ATK: 20
        Constants::BASE_PLAYER_DEF,     // DEF: 10
        Constants::BASE_PLAYER_CRIT,    // CRIT: 0.1f
        Constants::BASE_PLAYER_EVADE    // EVADE: 0.05f
    );
}

Stats Pikachu::getEvolvedStats() const {
    // Enhanced stats for "evolved" Pikachu
    Stats base = getBaseStats();
    return Stats(
        base.hp + 30,       // HP: 130
        base.mp + 20,       // MP: 70
        base.atk + 10,      // ATK: 30
        base.def + 5,       // DEF: 15
        base.crit + 0.05f,  // CRIT: 0.15f
        base.evade + 0.05f  // EVADE: 0.1f
    );
}

void Pikachu::setupPikachuSkills() {
    setupBaseSkills();
    
    // Set Pikachu-specific properties
    m_name = "Pikachu";
    m_level = 1;
    m_stats = getBaseStats();
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
}
