#pragma once
#include "Enemy.h"

class Bisasam : public Enemy {
public:
    Bisasam();
    virtual ~Bisasam();
    
    // Bisasam-specific behavior
    virtual CombatAction chooseAction(const Entity& target) const override;
    SkillType selectRandomSpecialSkill() const;
    bool attemptDodge() const; // 30% chance
    
    // Factory method
    static std::unique_ptr<Bisasam> create();
    
protected:
    virtual void setupSkills() override;
    
private:
    void setupBisasam();
};
