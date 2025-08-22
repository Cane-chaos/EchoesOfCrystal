#pragma once
#include "Enemy.h"

class Chalamander : public Enemy {
public:
    Chalamander();
    virtual ~Chalamander();
    
    // Chalamander-specific behavior
    virtual CombatAction chooseAction(const Entity& target) const override;
    SkillType selectRandomSpecialSkill() const;
    bool attemptDodge() const; // 30% chance
    
    // Factory method
    static std::unique_ptr<Chalamander> create();
    
protected:
    virtual void setupSkills() override;
    
private:
    void setupChalamander();
};
