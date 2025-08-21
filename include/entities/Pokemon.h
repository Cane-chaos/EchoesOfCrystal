#pragma once
#include "Entity.h"
#include "Skill.h"
#include <vector>
#include <memory>

class Pokemon : public Entity {
public:
    Pokemon(PokemonType type, const std::string& name);
    virtual ~Pokemon();
    
    // Basic info
    const std::string& getName() const { return m_name; }
    bool isEvolved() const { return m_evolved; }
    
    // Skills
    void addSkill(std::unique_ptr<Skill> skill);
    const std::vector<std::unique_ptr<Skill>>& getSkills() const { return m_skills; }
    std::vector<Skill*> getAvailableSkills() const;
    bool canUseSkill(SkillType skillType) const;
    
    // Combat
    int calculateDamage(const Skill& skill, const Entity& target) const;
    int calculateHealing(const Skill& skill) const;
    void useSkill(SkillType skillType, Entity& target);
    void normalAttack(Entity& target);
    
    // Cooldowns
    void updateCooldowns();
    void resetCooldowns();
    
    // Evolution
    virtual void evolve();
    
    // Type effectiveness
    float getTypeEffectiveness(PokemonType attackerType, PokemonType defenderType) const;
    
    // Factory methods
    static std::unique_ptr<Pokemon> createBlazeling();
    static std::unique_ptr<Pokemon> createAquary();
    static std::unique_ptr<Pokemon> createVerdil();
    
    // Entity overrides
    virtual void update(sf::Time dt) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
protected:
    virtual void setupBaseSkills() = 0;
    virtual void setupEvolutionSkill() = 0;
    virtual Stats getBaseStats() const = 0;
    virtual Stats getEvolvedStats() const = 0;
    
protected:
    std::string m_name;
    bool m_evolved;
    std::vector<std::unique_ptr<Skill>> m_skills;
    
    // Visual representation
    sf::RectangleShape m_sprite;
};

// Specific Pokemon classes
class Blazeling : public Pokemon {
public:
    Blazeling();
    
protected:
    virtual void setupBaseSkills() override;
    virtual void setupEvolutionSkill() override;
    virtual Stats getBaseStats() const override;
    virtual Stats getEvolvedStats() const override;
};

class Aquary : public Pokemon {
public:
    Aquary();
    
protected:
    virtual void setupBaseSkills() override;
    virtual void setupEvolutionSkill() override;
    virtual Stats getBaseStats() const override;
    virtual Stats getEvolvedStats() const override;
};

class Verdil : public Pokemon {
public:
    Verdil();
    
protected:
    virtual void setupBaseSkills() override;
    virtual void setupEvolutionSkill() override;
    virtual Stats getBaseStats() const override;
    virtual Stats getEvolvedStats() const override;
};
