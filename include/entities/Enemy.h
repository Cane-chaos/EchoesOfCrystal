#pragma once
#include "Entity.h"
#include "Skill.h"
#include <vector>
#include <memory>

class Enemy : public Entity {
public:
    Enemy(const std::string& name, PokemonType type = PokemonType::Fire);
    virtual ~Enemy();
    
    // Basic info
    const std::string& getName() const { return m_name; }
    
    // AI behavior
    virtual CombatAction chooseAction(const Entity& target) const;
    virtual void performAction(CombatAction action, Entity& target);
    
    // Skills
    void addSkill(std::unique_ptr<Skill> skill);
    const std::vector<std::unique_ptr<Skill>>& getSkills() const { return m_skills; }
    std::vector<Skill*> getAvailableSkills() const;
    
    // Combat
    void normalAttack(Entity& target);
    void useSkill(SkillType skillType, Entity& target);
    
    // Cooldowns
    void updateCooldowns();
    
    // Factory methods
    static std::unique_ptr<Enemy> createRegularEnemy();
    
    // Entity overrides
    virtual void update(sf::Time dt) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
protected:
    virtual void setupSkills();
    
protected:
    std::string m_name;
    std::vector<std::unique_ptr<Skill>> m_skills;
    
    // Visual representation
    sf::RectangleShape m_sprite;
};
