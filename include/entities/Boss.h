#pragma once
#include "Enemy.h"

class Boss : public Enemy {
public:
    Boss();
    virtual ~Boss();
    
    // Boss-specific behavior
    virtual CombatAction chooseAction(const Entity& target) const override;
    
    // Passive abilities
    virtual int modifyIncomingDamage(int damage) const;
    
    // Factory method
    static std::unique_ptr<Boss> createBoss();
    
protected:
    virtual void setupSkills() override;
    
private:
    void setupBossStats();
};
