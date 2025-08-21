# Mini Game C++/SFML - Technical Design Document

## 1. Technology & Architecture

### Core Technologies
- **Language**: C++17
- **Build System**: CMake 3.16+
- **Graphics Library**: SFML 2.6.x (graphics, window, audio)
- **Screen Resolution**: 1280×720, tileSize = 32px
- **Target FPS**: 60 (deltaTime-based)

### Architecture Pattern
**Finite State Machine** with the following states:
- `MenuState` - Start/Exit menu
- `MapState` - Board navigation
- `DiceState` - Dice rolling animation
- `CoinState` - Coin flip mechanics
- `CombatState` - Turn-based combat
- `GameOverState` - Death/restart screen
- `VictoryState` - Win condition screen
- `PauseState` - Optional pause functionality

### Core Classes
```
Core:
├── Game (main game loop, state management)
├── State (abstract base for all states)
├── StateStack (manages state transitions)
├── RNG (random number generation)
├── AudioManager (sound/music management)
├── AssetManager (resource loading)
└── SaveSystem (JSON save/load)

World/UI:
├── Map (30×30 board management)
├── Tile (individual tile properties)
├── UI::Button (clickable buttons)
├── UI::Panel (UI containers)
├── UI::TextLabel (text display)
└── UI::Bar (HP/MP bars)

Entities:
├── Entity (base entity class)
├── Player (player character)
├── Enemy (regular enemies)
├── Boss (boss enemy)
├── Pokemon (player's pokemon)
└── Skill (combat abilities)
```

## 2. Map & Movement System

### Board Layout (30×30)
- **Start Position**: (1,1)
- **Goal Position**: (28,28) - Boss tile
- **Tile Types**:
  - `Empty` - Safe tiles
  - `Rock` - Blocks movement
  - `Pit` - Coin flip challenge
  - `Rift` - Dice roll teleport
  - `Arrow{Up/Down/Left/Right}` - Direction change
  - `EnemyTile` - Combat encounter
  - `BossTile` - Final boss at goal

### Tile Distribution (per 20 walkable tiles)
- Enemy: 30%
- Pit: 15%
- Rift: 10%
- Rock: 15%
- Arrow: 15%
- Empty: 15%

### Movement Mechanics
```cpp
// Pseudocode for movement
int steps = roll_d6(); // 1..6
Vec2 target = advance(currentPos, dir, steps, mapBounds);
if (tileAt(target) == ROCK) pos = stepBefore(target, dir);
else pos = target;

switch (tileAt(pos)) {
  case PIT: if (!coinGuessSuccess()) return GameOver(); else stay();
            break;
  case RIFT: {
      int r = roll_d6();
      if (r == 1) return GameOver();
      pos = randomValidTile();
      resolveLanding(pos); // trigger new tile effect
    } break;
  case ARROW: dir = arrowDirection(pos); break;
  case ENEMY: enterCombat(ENEMY); break;
  case BOSS: enterCombat(BOSS); break;
  default: ; // Empty
}
```

## 3. RNG & Animation Systems

### Dice System
- **Range**: 1d6 (uniform distribution)
- **Animation**: 800ms duration with rapid number changes
- **No rerolls**: Single result per activation

### Coin System
- **Probability**: 50/50 Head/Tail
- **Player Choice**: Select Head/Tail before flip
- **Animation**: 2-3 seconds (Fear & Hunger style)
- **No rerolls**: Single result per flip

## 4. Combat System

### Turn Order
Player → Enemy → repeat until one side reaches HP ≤ 0

### Base Stats
```
Player (Pokemon): HP=100, MP=50, ATK=20, DEF=10, Crit=10%, Evade=5%
Enemy (Regular): HP=70, MP=30, ATK=15, DEF=8
Boss: HP=180, MP=60, ATK=25, DEF=12, Passive: "Resolute" (-10% damage)
```

### Damage Formula
```cpp
base = max(1, ATK_attacker - DEF_defender)
rand = uniform(0.9, 1.1)
crit = 1.5 if critical hit (Crit% chance)
element = type_effectiveness_multiplier // 1.5/0.5/1.0
damage = floor(base * rand * crit * element)
```

### Combat Flow
```cpp
while (player.alive() && foe.alive()) {
  // Coin before player's turn
  bool allowSkill = coinFlipWithGuess();
  Action act = allowSkill ? chooseSkillOrNormal(player) : NormalAttack();
  resolve(act, player, foe);

  if (!foe.alive()) break;

  Action enemyAct = enemyAI(foe, player);
  resolve(enemyAct, foe, player);
}
```

## 5. Skills & Type System

### Skill List
| Skill | MP Cost | Cooldown | Effect | Damage/Heal |
|-------|---------|----------|--------|-------------|
| Power Strike | 8 | 1 turn | Single target | ATK × 1.6 |
| Cleave | 12 | 2 turns | AOE attack | ATK × 1.2 |
| Heal | 10 | 2 turns | Self heal | ATK × 0.8 HP |
| Guard | 6 | 2 turns | Damage reduction | 0.6× damage for 1 turn |
| Counter | 10 | 3 turns | Reflect damage | 30% damage reflection |

### Type Effectiveness
```
Fire → Grass: 1.5×
Water → Fire: 1.5×
Grass → Water: 1.5×
All others: 1.0×
```

## 6. Pokemon & Progression

### Starter Pokemon
- **Blazeling (Fire)**: High ATK growth, low DEF
- **Aquary (Water)**: Balanced stats, good MP
- **Verdil (Grass)**: High DEF, moderate ATK

### Growth System
- **Per Victory**: +1 Level → +2 ATK, +1 DEF, +5 HP, +2 MP
- **Evolution**: Level 5 unlocks signature skill
- **Upgrade Points**: Every 2 victories = 2 points to distribute

## 7. Save System

### Save Data (JSON)
```json
{
  "mapSeed": 12345,
  "position": {"x": 5, "y": 7},
  "direction": "UP",
  "level": 3,
  "stats": {"atk": 24, "def": 13, "hp": 115, "mp": 56},
  "currentHP": 89,
  "currentMP": 32,
  "skillCooldowns": {"powerStrike": 0, "cleave": 1},
  "victories": 8,
  "rngSeed": 67890
}
```

## 8. Input Controls
- **Enter**: Roll dice (Map), Confirm (UI)
- **Mouse/Arrow Keys**: Menu/skill selection
- **Esc**: Pause/Back

## 9. Audio System
- `bgm_menu.ogg` - Menu background music
- `bgm_map.ogg` - Map exploration music
- `bgm_combat.ogg` - Combat music
- `sfx_coin.ogg` - Coin flip sound
- `sfx_dice.ogg` - Dice roll sound
- `sfx_hit.ogg` - Attack sound effects
