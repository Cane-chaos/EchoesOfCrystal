#include "entities/Pokemon.h"
#include "Constants.h"

Pokemon::Pokemon(PokemonType type, const std::string& name)
    : m_name(name)
    , m_evolved(false)
{
    setType(type);
    m_sprite.setSize(sf::Vector2f(Constants::TILE_SIZE * 0.8f, Constants::TILE_SIZE * 0.8f));
    m_sprite.setOrigin(m_sprite.getSize().x / 2.0f, m_sprite.getSize().y / 2.0f);
}

Pokemon::~Pokemon() {
}

void Pokemon::addSkill(std::unique_ptr<Skill> skill) {
    m_skills.push_back(std::move(skill));
}

std::vector<Skill*> Pokemon::getAvailableSkills() const {
    std::vector<Skill*> available;
    for (const auto& skill : m_skills) {
        if (skill->canUse(m_currentMP)) {
            available.push_back(skill.get());
        }
    }
    return available;
}

bool Pokemon::canUseSkill(SkillType skillType) const {
    for (const auto& skill : m_skills) {
        if (skill->getType() == skillType && skill->canUse(m_currentMP)) {
            return true;
        }
    }
    return false;
}

int Pokemon::calculateDamage(const Skill& skill, const Entity& target) const {
    int baseDamage = skill.calculateDamage(m_stats.atk);
    float effectiveness = getTypeEffectiveness(m_type, target.getType());
    return static_cast<int>(baseDamage * effectiveness);
}

int Pokemon::calculateHealing(const Skill& skill) const {
    return skill.calculateHealing(m_stats.atk);
}

void Pokemon::useSkill(SkillType skillType, Entity& target) {
    for (auto& skill : m_skills) {
        if (skill->getType() == skillType && skill->canUse(m_currentMP)) {
            modifyMP(-skill->getData().mpCost);
            skill->use();
            
            if (skill->getData().isOffensive) {
                int damage = calculateDamage(*skill, target);
                target.modifyHP(-damage);
            } else if (skill->getData().healMultiplier > 0) {
                int healing = calculateHealing(*skill);
                modifyHP(healing);
            }
            
            if (skill->hasSpecialEffect()) {
                skill->applySpecialEffect(*this, target);
            }
            break;
        }
    }
}

void Pokemon::normalAttack(Entity& target) {
    int damage = m_stats.atk - target.getStats().def;
    damage = std::max(1, damage);
    target.modifyHP(-damage);
}

void Pokemon::updateCooldowns() {
    for (auto& skill : m_skills) {
        skill->updateCooldown();
    }
}

void Pokemon::resetCooldowns() {
    for (auto& skill : m_skills) {
        skill->resetCooldown();
    }
}

void Pokemon::evolve() {
    if (!m_evolved) {
        m_evolved = true;
        m_stats = getEvolvedStats();
        setupEvolutionSkill();
    }
}

float Pokemon::getTypeEffectiveness(PokemonType attackerType, PokemonType defenderType) const {
    if (attackerType == PokemonType::Fire && defenderType == PokemonType::Grass) {
        return Constants::TYPE_SUPER_EFFECTIVE;
    }
    if (attackerType == PokemonType::Water && defenderType == PokemonType::Fire) {
        return Constants::TYPE_SUPER_EFFECTIVE;
    }
    if (attackerType == PokemonType::Grass && defenderType == PokemonType::Water) {
        return Constants::TYPE_SUPER_EFFECTIVE;
    }
    
    if (attackerType == PokemonType::Fire && defenderType == PokemonType::Water) {
        return Constants::TYPE_NOT_VERY_EFFECTIVE;
    }
    if (attackerType == PokemonType::Water && defenderType == PokemonType::Grass) {
        return Constants::TYPE_NOT_VERY_EFFECTIVE;
    }
    if (attackerType == PokemonType::Grass && defenderType == PokemonType::Fire) {
        return Constants::TYPE_NOT_VERY_EFFECTIVE;
    }
    
    return Constants::TYPE_NORMAL_EFFECTIVE;
}

std::unique_ptr<Pokemon> Pokemon::createBlazeling() {
    return std::make_unique<Blazeling>();
}

std::unique_ptr<Pokemon> Pokemon::createAquary() {
    return std::make_unique<Aquary>();
}

std::unique_ptr<Pokemon> Pokemon::createVerdil() {
    return std::make_unique<Verdil>();
}

void Pokemon::update(sf::Time dt) {
    (void)dt; // Not used yet
    // Update world position of the sprite here (not in draw)
    m_sprite.setPosition(m_position);
    // Update animations, effects, etc.
}

void Pokemon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Do not mutate state in const draw
    target.draw(m_sprite, states);
}

// Blazeling implementation
Blazeling::Blazeling() : Pokemon(PokemonType::Fire, "Blazeling") {
    m_stats = getBaseStats();
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    m_sprite.setFillColor(sf::Color::Red);
    setupBaseSkills();
}

void Blazeling::setupBaseSkills() {
    addSkill(Skill::createPowerStrike());
    addSkill(Skill::createHeal());
}

void Blazeling::setupEvolutionSkill() {
    addSkill(Skill::createFireball());
}

Stats Blazeling::getBaseStats() const {
    return Stats(95, 45, 22, 8, 0.12f, 0.05f); // High ATK, low DEF
}

Stats Blazeling::getEvolvedStats() const {
    return Stats(120, 60, 28, 12, 0.15f, 0.07f);
}

// Aquary implementation
Aquary::Aquary() : Pokemon(PokemonType::Water, "Aquary") {
    m_stats = getBaseStats();
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    m_sprite.setFillColor(sf::Color::Blue);
    setupBaseSkills();
}

void Aquary::setupBaseSkills() {
    addSkill(Skill::createCleave());
    addSkill(Skill::createGuard());
}

void Aquary::setupEvolutionSkill() {
    addSkill(Skill::createAquaPulse());
}

Stats Aquary::getBaseStats() const {
    return Stats(100, 55, 20, 10, 0.10f, 0.05f); // Balanced
}

Stats Aquary::getEvolvedStats() const {
    return Stats(130, 75, 26, 15, 0.12f, 0.07f);
}

// Verdil implementation
Verdil::Verdil() : Pokemon(PokemonType::Grass, "Verdil") {
    m_stats = getBaseStats();
    m_currentHP = m_stats.hp;
    m_currentMP = m_stats.mp;
    m_sprite.setFillColor(sf::Color::Green);
    setupBaseSkills();
}

void Verdil::setupBaseSkills() {
    addSkill(Skill::createCounter());
    addSkill(Skill::createHeal());
}

void Verdil::setupEvolutionSkill() {
    addSkill(Skill::createLeafBlade());
}

Stats Verdil::getBaseStats() const {
    return Stats(105, 50, 18, 12, 0.08f, 0.05f); // High DEF, moderate ATK
}

Stats Verdil::getEvolvedStats() const {
    return Stats(135, 70, 24, 18, 0.10f, 0.07f);
}
