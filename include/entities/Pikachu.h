#pragma once
#include "Pokemon.h"

class Pikachu : public Pokemon {
public:
    Pikachu();
    virtual ~Pikachu();
    
    // Pikachu-specific methods
    std::vector<SkillType> getAttackSkills() const;
    std::vector<SkillType> getDefenseSkills() const;
    
    // Factory method
    static std::unique_ptr<Pikachu> create();
    
protected:
    virtual void setupBaseSkills() override;
    virtual void setupEvolutionSkill() override;
    virtual Stats getBaseStats() const override;
    virtual Stats getEvolvedStats() const override;
    
private:
    void setupPikachuSkills();
};
