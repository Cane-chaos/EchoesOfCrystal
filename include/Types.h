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
    EnhancedCombat,
    Ready,
    GameOver,
    Victory,
    Pause
};

enum class TileType {
    Empty,          // Walkable path
    Wall,           // Impassable walls
    Rock,           // Breakable obstacles (2 turns: break + move)
    PortalA,        // Portal A (teleports to PortalB)
    PortalB,        // Portal B (teleports to PortalA)
    Enemy,          // Normal monsters (Charmander, Bulbasaur)
    Boss,           // Boss monster at goal
    Goal            // Goal position
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

enum class CombatResult {
    None,
    Victory,
    Unfortunately
};

enum class SkillType {
    // Normal attack
    NormalAttack,

    // Pikachu skills (6 total: 3 attack + 3 defense)
    ThunderBolt,     // Attack 1
    QuickAttack,     // Attack 2
    IronTail,        // Attack 3
    Agility,         // Defense 1 (speed boost)
    DoubleTeam,      // Defense 2 (evasion)
    Substitute,      // Defense 3 (damage reduction)

    // Monster skills
    Ember,           // Chalamander attack
    VineWhip,        // Bisasam attack
    Tackle,          // Common attack
    Dodge,           // Common defense (30% evade)

    // Legacy skills kept for compatibility with existing code
    PowerStrike,
    Cleave,
    Heal,
    Guard,
    Counter,
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

// CombatResult enum moved to line 48

enum class CombatPhase {
    Ready,           // "Are you Ready!?" screen (3s)
    PlayerCoin,      // Player tosses coin for attack
    PlayerAction,    // Player selects action based on coin
    EnemyCoin,       // Player tosses coin for defense
    EnemyAction,     // Enemy attacks, player defends
    Victory,         // Show victory screen
    Defeat,          // Show defeat screen
    Ended
};

enum class MonsterType {
    Pikachu,         // Main character
    Chalamander,     // Normal monster 1
    Bisasam,         // Normal monster 2
    Boss             // Boss monster (TBD)
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

struct TeleportGate {
    Vec2i position;
    int pairId;          // ID of paired gate
    Vec2i targetPosition; // Position of paired gate

    TeleportGate() = default;
    TeleportGate(Vec2i pos, int id) : position(pos), pairId(id) {}
};

struct RockState {
    Vec2i position;
    bool isBroken;       // false = intact, true = broken (passable)
    int breakProgress;   // 0 = intact, 1 = breaking, 2 = broken

    RockState() = default;
    RockState(Vec2i pos) : position(pos), isBroken(false), breakProgress(0) {}
};

// Game mechanics structs
struct DiceRoll {
    int result;
    int remainingSteps;
    bool isActive;

    DiceRoll() : result(0), remainingSteps(0), isActive(false) {}
    DiceRoll(int r) : result(r), remainingSteps(r), isActive(true) {}
};

struct Portal {
    Vec2i positionA;
    Vec2i positionB;
    bool isActive;

    Portal() = default;
    Portal(Vec2i a, Vec2i b) : positionA(a), positionB(b), isActive(true) {}
};

struct MonsterData {
    Vec2i position;
    MonsterType type;
    bool isDefeated;

    MonsterData() = default;
    MonsterData(Vec2i pos, MonsterType t) : position(pos), type(t), isDefeated(false) {}
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
    std::vector<RockState> rockStates;
    std::vector<TeleportGate> teleportGates;
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
