# Mini Game C++/SFML - Board × Combat Game

A complete C++17/SFML mini-game featuring board-based movement with turn-based Pokémon-style combat.

## 🎮 Game Features

### Core Gameplay
- **30×30 Board Navigation**: Move through a procedurally generated map
- **Dice-Based Movement**: Roll 1d6 to move in your current direction
- **Multiple Tile Types**: Empty, Rock, Pit, Rift, Arrow, Enemy, Boss
- **Turn-Based Combat**: Pokémon-style battles with skills and type effectiveness
- **Coin Flip Mechanics**: Determine available actions in combat
- **Three Starter Pokémon**: Fire (Blazeling), Water (Aquary), Grass (Verdil)

### Combat System
- **Skill-Based Combat**: 5+ unique skills with cooldowns and MP costs
- **Type Effectiveness**: Fire > Grass > Water > Fire triangle
- **Progression System**: Level up, evolve at Level 5, upgrade stats
- **Boss Battles**: Special encounter at the goal position

### Technical Features
- **Finite State Machine**: Clean state management (Menu, Map, Dice, Coin, Combat, etc.)
- **Save/Load System**: JSON-based game persistence
- **Audio Support**: Background music and sound effects
- **Modular Architecture**: Easy to extend and modify

## 🏗️ Architecture

### State Machine
```
MenuState ──→ MapState ──→ DiceState
    ↑           ↓              ↓
    └─────── GameOverState ←───┘
            VictoryState
            CombatState
            CoinState
            PauseState
```

### Core Systems
- **Game**: Main game loop and state management
- **StateStack**: Manages state transitions and updates
- **RNG**: Centralized random number generation
- **AudioManager**: Sound and music management
- **AssetManager**: Resource loading and caching
- **SaveSystem**: JSON save/load functionality

### Entity System
- **Entity**: Base class for all game objects
- **Player**: Player character with Pokémon
- **Pokemon**: Combat entities with skills and types
- **Enemy/Boss**: AI-controlled opponents
- **Skill**: Combat abilities with various effects

## 🛠️ Build Requirements

### Dependencies
- **CMake 3.16+**
- **C++17 compatible compiler**
- **SFML 2.6.x** (graphics, window, audio)
- **nlohmann/json 3.2.0+**

### Quick Start
```bash
# Install dependencies (example for Ubuntu)
sudo apt install build-essential cmake libsfml-dev nlohmann-json3-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./MiniGameSFML
```

See [BUILD.md](BUILD.md) for detailed platform-specific instructions.

## 🎯 Game Mechanics

### Movement System
```cpp
// Pseudocode for movement
int steps = roll_d6(); // 1..6
Vec2 target = advance(currentPos, dir, steps, mapBounds);
if (tileAt(target) == ROCK) pos = stepBefore(target, dir);
else pos = target;

switch (tileAt(pos)) {
  case PIT: if (!coinGuessSuccess()) return GameOver(); else stay();
  case RIFT: {
      int r = roll_d6();
      if (r == 1) return GameOver();
      pos = randomValidTile();
      resolveLanding(pos);
    }
  case ARROW: dir = arrowDirection(pos);
  case ENEMY: enterCombat(ENEMY);
  case BOSS: enterCombat(BOSS);
}
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

### Damage Formula
```cpp
base = max(1, ATK_attacker - DEF_defender)
rand = uniform(0.9, 1.1)
crit = 1.5 if critical hit (Crit% chance)
element = type_effectiveness_multiplier // 1.5/0.5/1.0
damage = floor(base * rand * crit * element)
```

## 📊 Skills & Progression

### Available Skills
| Skill | MP Cost | Cooldown | Effect | Damage/Heal |
|-------|---------|----------|--------|-------------|
| Power Strike | 8 | 1 turn | Single target | ATK × 1.6 |
| Cleave | 12 | 2 turns | AOE attack | ATK × 1.2 |
| Heal | 10 | 2 turns | Self heal | ATK × 0.8 HP |
| Guard | 6 | 2 turns | Damage reduction | 0.6× damage for 1 turn |
| Counter | 10 | 3 turns | Reflect damage | 30% damage reflection |

### Evolution Skills (Level 5+)
- **Fireball** (Fire): ATK × 1.4, 20% burn chance
- **Aqua Pulse** (Water): ATK × 1.4, 20% slow chance  
- **Leaf Blade** (Grass): ATK × 1.4, 20% poison chance

### Type Effectiveness
```
Fire → Grass: 1.5×
Water → Fire: 1.5×
Grass → Water: 1.5×
Reverse matchups: 0.5×
Same/neutral: 1.0×
```

## 🎮 Controls

- **Enter**: Roll dice (Map), Confirm (UI)
- **Arrow Keys/Mouse**: Navigate menus and select options
- **Esc**: Pause/Back
- **Mouse**: Click buttons and UI elements

## 📁 Project Structure

```
PROJECT/
├── CMakeLists.txt
├── README.md
├── BUILD.md
├── DESIGN.md
├── assets/
│   ├── fonts/
│   └── audio/
├── include/
│   ├── Game.h
│   ├── StateStack.h
│   ├── Constants.h
│   ├── Types.h
│   ├── core/
│   ├── states/
│   ├── world/
│   ├── entities/
│   └── ui/
└── src/
    ├── main.cpp
    ├── Game.cpp
    ├── StateStack.cpp
    ├── core/
    ├── states/
    ├── world/
    ├── entities/
    └── ui/
```

## 🔧 Customization

### Adding New Tiles
1. Add tile type to `TileType` enum in `Types.h`
2. Implement behavior in `Map::handleTileEvent()`
3. Add visual representation in `Map::getTileColor()`

### Adding New Skills
1. Add skill type to `SkillType` enum
2. Create factory method in `Skill` class
3. Add to Pokémon skill sets in `setupBaseSkills()`

### Modifying Map Generation
- Edit tile distribution percentages in `Constants.h`
- Modify generation algorithm in `Map::distributeTiles()`
- Create custom maps via CSV files

## 🐛 Known Limitations

- Audio files are optional (game runs without them)
- Default font fallback may not render text on some systems
- Map generation is basic (no advanced algorithms)
- AI is simple (no advanced strategies)

## 🚀 Future Enhancements

- [ ] Advanced map generation algorithms
- [ ] More Pokémon types and skills
- [ ] Inventory and item system
- [ ] Multiple save slots
- [ ] Animated sprites and effects
- [ ] Network multiplayer support

## 📄 License

This project is provided as educational material. Feel free to use and modify for learning purposes.

## 🤝 Contributing

This is a complete skeleton project. You can:
1. Add new features and mechanics
2. Improve graphics and animations
3. Enhance AI and game balance
4. Add more content (skills, Pokémon, tiles)

See the code comments and design document for implementation details.
