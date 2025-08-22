#pragma once
#include "Types.h"
#include <string>
#include <memory>

class Skill {
public:
    Skill(SkillType type, const std::string& name, const SkillData& data);
    ~Skill();
    
    // Basic info
    SkillType getType() const { return m_type; }
    const std::string& getName() const { return m_name; }
    const SkillData& getData() const { return m_data; }
    
    // Usage
    bool canUse(int currentMP) const;
    bool isOnCooldown() const { return m_currentCooldown > 0; }
    int getCurrentCooldown() const { return m_currentCooldown; }
    
    // Cooldown management
    void use();
    void updateCooldown();
    void resetCooldown();
    
    // Damage/healing calculation
    int calculateDamage(int attackerATK) const;
    int calculateHealing(int attackerATK) const;
    
    // Special effects
    bool hasSpecialEffect() const { return m_data.hasSpecialEffect; }
    void applySpecialEffect(class Entity& user, class Entity& target) const;
    
    // Factory methods for creating skills
    // Legacy skills
    static std::unique_ptr<Skill> createPowerStrike();
    static std::unique_ptr<Skill> createCleave();
    static std::unique_ptr<Skill> createHeal();
    static std::unique_ptr<Skill> createGuard();
    static std::unique_ptr<Skill> createCounter();
    static std::unique_ptr<Skill> createFireball();
    static std::unique_ptr<Skill> createAquaPulse();
    static std::unique_ptr<Skill> createLeafBlade();

    // Pikachu Attack Skills
    static std::unique_ptr<Skill> createThunderBolt();
    static std::unique_ptr<Skill> createQuickAttack();
    static std::unique_ptr<Skill> createIronTail();

    // Pikachu Defense Skills
    static std::unique_ptr<Skill> createAgility();
    static std::unique_ptr<Skill> createDoubleTeam();
    static std::unique_ptr<Skill> createSubstitute();

    // Monster Skills
    static std::unique_ptr<Skill> createEmber();
    static std::unique_ptr<Skill> createVineWhip();
    static std::unique_ptr<Skill> createTackle();
    static std::unique_ptr<Skill> createDodge();
    
private:
    SkillType m_type;
    std::string m_name;
    SkillData m_data;
    int m_currentCooldown;
};
