#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

// Type aliases
using Vec2i = sf::Vector2i;
using Vec2f = sf::Vector2f;

// Enumerations
enum class StateID {
    Menu,
    Map,
    Dice,
    Coin,
    Combat,
    GameOver,
    Victory,
    Pause
};

enum class TileType {
    Empty,
    Rock,
    Pit,
    Rift,
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    Enemy,
    Boss
};

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

enum class PokemonType {
    Fire,
    Water,
    Grass
};

enum class SkillType {
    PowerStrike,
    Cleave,
    Heal,
    Guard,
    Counter,
    // Evolution skills
    Fireball,
    AquaPulse,
    LeafBlade
};

enum class CombatAction {
    NormalAttack,
    UseSkill,
    Guard,
    None
};

enum class CoinChoice {
    Head,
    Tail
};

enum class CombatResult {
    PlayerWin,
    EnemyWin,
    Ongoing
};

// Structs
struct Stats {
    int hp = 0;
    int mp = 0;
    int atk = 0;
    int def = 0;
    float crit = 0.0f;
    float evade = 0.0f;
    
    Stats() = default;
    Stats(int h, int m, int a, int d, float c = 0.0f, float e = 0.0f)
        : hp(h), mp(m), atk(a), def(d), crit(c), evade(e) {}
};

struct SkillData {
    SkillType type;
    int mpCost;
    int cooldown;
    float damageMultiplier;
    float healMultiplier;
    bool isOffensive;
    bool isDefensive;
    bool hasSpecialEffect;
    
    SkillData() = default;
    SkillData(SkillType t, int mp, int cd, float dmg = 0.0f, float heal = 0.0f, 
              bool off = true, bool def = false, bool special = false)
        : type(t), mpCost(mp), cooldown(cd), damageMultiplier(dmg), 
          healMultiplier(heal), isOffensive(off), isDefensive(def), hasSpecialEffect(special) {}
};

struct SaveData {
    unsigned int mapSeed;
    Vec2i position;
    Direction direction;
    int level;
    Stats baseStats;
    int currentHP;
    int currentMP;
    std::vector<int> skillCooldowns;
    int victories;
    unsigned int rngSeed;
    PokemonType pokemonType;
    int upgradePoints;
};

// Utility functions
inline Direction vectorToDirection(const Vec2i& vec) {
    if (vec.x > 0) return Direction::Right;
    if (vec.x < 0) return Direction::Left;
    if (vec.y > 0) return Direction::Down;
    return Direction::Up;
}

inline Vec2i directionToVector(Direction dir) {
    switch (dir) {
        case Direction::Up: return Vec2i(0, -1);
        case Direction::Down: return Vec2i(0, 1);
        case Direction::Left: return Vec2i(-1, 0);
        case Direction::Right: return Vec2i(1, 0);
    }
    return Vec2i(0, 0);
}

inline const char* directionToString(Direction dir) {
    switch (dir) {
        case Direction::Up: return "UP";
        case Direction::Down: return "DOWN";
        case Direction::Left: return "LEFT";
        case Direction::Right: return "RIGHT";
    }
    return "UNKNOWN";
}

inline const char* pokemonTypeToString(PokemonType type) {
    switch (type) {
        case PokemonType::Fire: return "Fire";
        case PokemonType::Water: return "Water";
        case PokemonType::Grass: return "Grass";
    }
    return "Unknown";
}
