#include "states/MapState.h"
#include "states/CombatState.h"
#include "StateStack.h"
#include "Constants.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include "core/AssetManager.h"
#include "core/RNG.h"
#include "core/SaveSystem.h"
#include <algorithm>
#include <string>
#include <iostream>

MapState::MapState(StateStack& stack, Context context)
    : State(stack, context)
    , m_canRoll(true)
    , m_remainingSteps(0)
    , m_autoPathActive(false)
    , m_stepTimer(sf::Time::Zero)
    , m_stepDelay(sf::milliseconds(500))  // 0.5s per step
    , m_justTeleported(false)
    , m_combatMonsterPos(0, 0)
{
    setupUI();

    // Initialize new game by default
    initializeNewGame(PokemonType::Fire);
}

void MapState::draw() {
    sf::RenderWindow& window = *getContext().window;

    // Set the game view
    window.setView(m_gameView);

    // Draw map with sprites
    m_map.drawWithSprites(window, m_gameView, *getContext().assets);

    // Draw visited tiles overlay
    m_map.drawVisitedTiles(window, m_player);

    // Draw player sprite
    m_map.drawPlayer(window, m_player.getMapPosition(), *getContext().assets);

    // Reset to default view for UI
    window.setView(window.getDefaultView());

    // Draw UI
    m_positionLabel.draw(window);
    m_directionLabel.draw(window);
    m_levelLabel.draw(window);
    m_victoriesLabel.draw(window);
    m_rollButton.draw(window);
    m_hpBar.draw(window);
    m_mpBar.draw(window);
}

bool MapState::update(sf::Time dt) {
    // Check for combat result from CombatState
    if (CombatState::s_combatJustEnded) {
        std::cout << "Combat just ended, processing result..." << std::endl;
        onCombatEnded(CombatState::s_lastCombatResult);

        // Reset the flag
        CombatState::s_combatJustEnded = false;
        CombatState::s_lastCombatResult = CombatResult::None;
    }

    m_player.update(dt);
    m_rollButton.update(dt);
    m_hpBar.update(dt);
    m_mpBar.update(dt);

    // Update auto-path system
    updateAutoPath(dt);

    updateUI();
    centerCameraOnPlayer();

    return false;
}

bool MapState::handleEvent(const sf::Event& event) {
    // Handle UI events
    if (m_rollButton.handleEvent(event)) {
        return true;
    }

    // Handle keyboard input
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter && m_canRoll) {
            rollDice();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            requestStackPush(StateID::Pause);
            return true;
        }
        // Combat result is now handled automatically through static variables
    }

    return false;
}

void MapState::initializeNewGame(PokemonType pokemonType) {
    // TODO: Use pokemonType to create appropriate Pokemon
    (void)pokemonType; // Suppress unused parameter warning

    // Generate fixed map with single path from Start to Boss
    m_map.generateFixedMap();

    // Initialize player at Start position (1,1)
    m_player.setMapPosition(Vec2i(1, 1));
    m_player.setDirection(Direction::Right);
    m_player.initVisitedGlobal(30, 30);
    m_player.markVisited(Vec2i(1, 1));  // Mark starting position

    // Set player position in world coordinates
    Vec2i mapPos = m_player.getMapPosition();
    m_player.setPosition(
        mapPos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
        mapPos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
    );

    m_canRoll = true;

    // Reset auto-path state
    m_remainingSteps = 0;
    m_autoPathActive = false;
    m_justTeleported = false;
}

void MapState::loadGame(const SaveData& saveData) {
    // Load map from save data
    // Load fixed map (same layout always)
    m_map.generateFixedMap();

    // Load player data
    m_player.loadFromSaveData(saveData);

    // Set player position
    Vec2i mapPos = m_player.getMapPosition();
    m_player.setPosition(
        mapPos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
        mapPos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
    );

    m_canRoll = true;
}

void MapState::saveGame() {
    SaveData saveData = m_player.createSaveData();
    getContext().save->saveGame(saveData);
}

void MapState::rollDice() {
    if (!m_canRoll || m_autoPathActive) return;

    // Roll dice and start auto-path
    int diceResult = getContext().rng->rollD6();
    startAutoPath(diceResult);

    m_canRoll = false;
}

void MapState::movePlayer(int steps) {
    Vec2i currentPos = m_player.getMapPosition();
    Direction direction = m_player.getDirection();

    // Calculate target position
    Vec2i dirVector = directionToVector(direction);
    Vec2i targetPos = currentPos + dirVector * steps;

    // Handle bounds and obstacles
    if (!m_map.isValidPosition(targetPos)) {
        // Clamp to map bounds
        targetPos.x = std::max(0, std::min(targetPos.x, m_map.getWidth() - 1));
        targetPos.y = std::max(0, std::min(targetPos.y, m_map.getHeight() - 1));
    }

    // Check for rock blocking
    if (m_map.getTileType(targetPos) == TileType::Rock) {
        // Stop at the tile before the rock
        targetPos = currentPos + dirVector * (steps - 1);
        if (!m_map.isValidPosition(targetPos)) {
            targetPos = currentPos;
        }
    }

    // Move player
    m_player.setMapPosition(targetPos);
    m_player.setPosition(
        targetPos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
        targetPos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
    );

    // Handle tile events
    handleTileEvent(m_map.getTileType(targetPos));
}

void MapState::handleTileEvent(TileType tileType) {
    switch (tileType) {
        case TileType::Rock:
            // Rock obstacle - requires breaking (2 turns)
            handleRockObstacle();
            break;
        case TileType::Wall:
            // Wall - impassable (shouldn't reach here)
            break;
        case TileType::PortalA:
        case TileType::PortalB:
            // Portal - teleport to paired portal
            handlePortalTeleport();
            break;
        case TileType::Enemy:
            requestStackPush(StateID::Combat);
            break;
        case TileType::Boss:
            requestStackPush(StateID::Combat);
            break;
        case TileType::Goal:
            // Reached goal - victory!
            handleGoalReached();
            break;
        default:
            break;
    }

    m_canRoll = true;
}

void MapState::handleRockObstacle() {
    // TODO: Implement rock breaking mechanics (2 turns)
    // For now, just show a message
}

void MapState::handlePortalTeleport() {
    // TODO: Implement portal teleport mechanics
    // Move player to paired portal position + 1 step forward
}

void MapState::handleGoalReached() {
    // Show congratulations and return to menu
    requestStackPush(StateID::Victory);
}

// Auto-path system implementation
void MapState::startAutoPath(int steps) {
    m_remainingSteps = steps;
    m_autoPathActive = true;
    m_stepTimer = sf::Time::Zero;
    m_justTeleported = false;
}

void MapState::updateAutoPath(sf::Time dt) {
    if (!m_autoPathActive || m_remainingSteps <= 0) {
        if (m_autoPathActive && m_remainingSteps <= 0) {
            m_autoPathActive = false;
            m_canRoll = true;  // Allow next dice roll
        }
        return;
    }

    m_stepTimer += dt;
    if (m_stepTimer >= m_stepDelay) {
        m_stepTimer = sf::Time::Zero;

        // Try to take one step
        auto nextPos = pickNextPosition();
        if (nextPos.has_value()) {
            // Move to next position
            Vec2i oldPos = m_player.getMapPosition();
            m_player.setMapPosition(*nextPos);
            m_player.markVisited(*nextPos);
            std::cout << "Marked visited: (" << nextPos->x << "," << nextPos->y << ")" << std::endl;

            // Update world coordinates
            m_player.setPosition(
                nextPos->x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
                nextPos->y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
            );

            m_remainingSteps--;

            // Check for events after movement
            checkCombatTrigger();
            checkGoalReached();
        } else {
            // No valid move - end turn
            m_remainingSteps = 0;
            m_autoPathActive = false;
            m_canRoll = true;
        }
    }
}
std::optional<Vec2i> MapState::pickNextPosition() {
    Vec2i current = m_player.getMapPosition();

    // Priority directions: Right → Down → Up → Left
    const Vec2i DIRS[4] = {
        {1, 0},   // Right
        {0, 1},   // Down
        {0, -1},  // Up
        {-1, 0}   // Left
    };

    for (const auto& dir : DIRS) {
        Vec2i next = Vec2i(current.x + dir.x, current.y + dir.y);

        // Check bounds
        if (!m_map.isInsideBounds(next)) continue;

        // Check if already visited
        if (m_player.hasVisited(next)) {
            std::cout << "Skipping visited tile: (" << next.x << "," << next.y << ")" << std::endl;
            continue;
        }

        TileType tileType = m_map.getTileType(next);

        // Handle different tile types
        switch (tileType) {
            case TileType::Wall:
                continue; // Can't move through walls

            case TileType::Rock:
                if (m_remainingSteps >= 2) {
                    // Break rock and move into it (consumes 2 steps)
                    m_map.destroyRock(next);
                    m_remainingSteps -= 2; // Pre-consume steps
                    return next;
                } else {
                    // Can't break rock with only 1 step - invalid move
                    continue;
                }

            case TileType::PortalA:
            case TileType::PortalB:
                if (!m_justTeleported) {
                    auto teleportDest = m_map.teleportNext(next);
                    if (teleportDest.has_value() && !m_player.hasVisited(*teleportDest)) {
                        m_justTeleported = true;
                        return *teleportDest;
                    }
                }
                continue;

            case TileType::Goal:
                // Always allow reaching goal
                return next;

            case TileType::Empty:
            case TileType::Enemy:
                // Valid move
                return next;

            default:
                continue;
        }
    }

    // No valid move found
    return std::nullopt;
}

void MapState::checkCombatTrigger() {
    // Only check for combat during active auto-path movement
    if (!m_autoPathActive) {
        return;
    }

    Vec2i playerPos = m_player.getMapPosition();

    // Check distance to all monsters
    for (const auto& monsterPos : m_map.getMonsterPositions()) {
        int distance = std::abs(playerPos.x - monsterPos.x) + std::abs(playerPos.y - monsterPos.y);
        if (distance <= m_remainingSteps) {
            std::cout << "Combat triggered! Player at (" << playerPos.x << "," << playerPos.y
                      << ") vs Monster at (" << monsterPos.x << "," << monsterPos.y
                      << "), distance: " << distance << ", remaining steps: " << m_remainingSteps << std::endl;

            // Trigger combat
            m_autoPathActive = false;
            m_remainingSteps = 0; // Stop auto-path
            m_canRoll = false; // Disable dice rolling during combat

            // Store monster position for combat result handling
            m_combatMonsterPos = monsterPos;

            // Don't move player yet - keep at current position during combat

            // Determine enemy type based on position
            std::string enemyType = "chalamander"; // Default
            if (monsterPos == Vec2i(20, 5)) {
                enemyType = "chalamander"; // Charmander
            } else if (monsterPos == Vec2i(8, 22)) {
                enemyType = "bisasam"; // Bulbasaur
            }

            // Push combat state
            requestStackPush(StateID::Combat);
            return;
        }
    }
}

void MapState::checkGoalReached() {
    Vec2i playerPos = m_player.getMapPosition();
    Vec2i goalPos = m_map.getGoalPosition();

    // Check if at goal or close enough with remaining steps
    int distanceToGoal = std::abs(playerPos.x - goalPos.x) + std::abs(playerPos.y - goalPos.y);

    if (playerPos == goalPos || distanceToGoal < m_remainingSteps) {
        // Reached goal - show congratulations
        m_autoPathActive = false;
        m_remainingSteps = 0;
        handleGoalReached();
    }
}

void MapState::onCombatEnded(CombatResult result) {
    Vec2i currentPos = m_player.getMapPosition();

    std::cout << "Combat ended with result: " << (result == CombatResult::Victory ? "Victory" : "Unfortunately") << std::endl;
    std::cout << "Player position: " << currentPos.x << ", " << currentPos.y << std::endl;
    std::cout << "Monster position: " << m_combatMonsterPos.x << ", " << m_combatMonsterPos.y << std::endl;

    if (result == CombatResult::Victory) {
        // Step 1: Remove the defeated monster from map
        Vec2i defeatedMonsterPos = m_combatMonsterPos; // Use stored monster position

        // Remove monster from map's monster list
        auto& monsterPositions = m_map.getMonsterPositions();
        auto it = std::find(monsterPositions.begin(), monsterPositions.end(), defeatedMonsterPos);
        if (it != monsterPositions.end()) {
            monsterPositions.erase(it);
        }

        // Step 2: Move player to monster position and mark it visited
        m_player.setMapPosition(defeatedMonsterPos);
        m_player.setPosition(
            defeatedMonsterPos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
            defeatedMonsterPos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
        );
        m_player.markVisited(defeatedMonsterPos);

        std::cout << "Player moved to monster position: (" << defeatedMonsterPos.x << "," << defeatedMonsterPos.y << ")" << std::endl;

        // Step 3: Try to advance +1 position according to priority: Right → Down → Up → Left
        const Vec2i DIRS[4] = {
            {1, 0},   // Right
            {0, 1},   // Down
            {0, -1},  // Up
            {-1, 0}   // Left
        };

        for (const auto& dir : DIRS) {
            Vec2i nextPos = Vec2i(defeatedMonsterPos.x + dir.x, defeatedMonsterPos.y + dir.y);

            // Check if position is valid for advancement
            if (m_map.isInsideBounds(nextPos) &&
                !m_player.hasVisited(nextPos) &&
                m_map.getTileType(nextPos) != TileType::Wall &&
                m_map.getTileType(nextPos) != TileType::Rock) {

                // Move player to new position
                m_player.setMapPosition(nextPos);
                m_player.markVisited(nextPos);

                // Update world coordinates
                m_player.setPosition(
                    nextPos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
                    nextPos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
                );

                std::cout << "Advanced to position: (" << nextPos.x << "," << nextPos.y << ")" << std::endl;

                break; // Found valid position, stop searching
            }
        }
        // If no valid advancement position found, player stays at monster position
    }
    // If Unfortunately, player position remains unchanged (no monster removal)

    // Re-enable dice rolling and reset auto-path
    m_canRoll = true;
    m_autoPathActive = false;
    m_remainingSteps = 0;

    // Force UI update
    updateUI();

    std::cout << "Combat result processed. Dice rolling re-enabled." << std::endl;
}

void MapState::setupUI() {
    AssetManager& assets = *getContext().assets;

    // Position labels
    m_positionLabel.setPosition(sf::Vector2f(10, 10));
    m_positionLabel.setFont(assets.getDefaultFont());
    m_positionLabel.setCharacterSize(16);

    m_directionLabel.setPosition(sf::Vector2f(10, 30));
    m_directionLabel.setFont(assets.getDefaultFont());
    m_directionLabel.setCharacterSize(16);

    m_levelLabel.setPosition(sf::Vector2f(10, 50));
    m_levelLabel.setFont(assets.getDefaultFont());
    m_levelLabel.setCharacterSize(16);

    m_victoriesLabel.setPosition(sf::Vector2f(10, 70));
    m_victoriesLabel.setFont(assets.getDefaultFont());
    m_victoriesLabel.setCharacterSize(16);

    // Roll button
    m_rollButton.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 120, Constants::SCREEN_HEIGHT - 60));
    m_rollButton.setSize(sf::Vector2f(100, 40));
    m_rollButton.setText("Roll Dice");
    m_rollButton.setFont(assets.getDefaultFont());
    m_rollButton.setCallback([this]() { rollDice(); });

    // HP/MP bars
    m_hpBar.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 220, 10));
    m_hpBar.setSize(sf::Vector2f(200, 20));
    m_hpBar.setFont(assets.getDefaultFont());
    m_hpBar.setFillColor(sf::Color::Red);
    m_hpBar.setTextFormat("HP: {current}/{max}");

    m_mpBar.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 220, 35));
    m_mpBar.setSize(sf::Vector2f(200, 20));
    m_mpBar.setFont(assets.getDefaultFont());
    m_mpBar.setFillColor(sf::Color::Blue);
    m_mpBar.setTextFormat("MP: {current}/{max}");

    // Setup game view
    m_gameView.setSize(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    m_gameView.setCenter(Constants::SCREEN_WIDTH / 2.0f, Constants::SCREEN_HEIGHT / 2.0f);
}

void MapState::updateUI() {
    Vec2i pos = m_player.getMapPosition();
    m_positionLabel.setText("Position: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")");
    m_directionLabel.setText("Direction: " + std::string(directionToString(m_player.getDirection())));
    m_levelLabel.setText("Level: " + std::to_string(m_player.getLevel()));
    m_victoriesLabel.setText("Victories: " + std::to_string(m_player.getVictories()));

    m_hpBar.setValues(m_player.getCurrentHP(), m_player.getMaxHP());
    m_mpBar.setValues(m_player.getCurrentMP(), m_player.getMaxMP());

    m_rollButton.setEnabled(m_canRoll);
}

void MapState::centerCameraOnPlayer() {
    Vec2f playerPos = m_player.getPosition();

    // Fixed map size: 30x30 tiles * 32px = 960x960 pixels
    const float MAP_PIXEL_WIDTH = Constants::MAP_WIDTH * Constants::TILE_SIZE;
    const float MAP_PIXEL_HEIGHT = Constants::MAP_HEIGHT * Constants::TILE_SIZE;

    // Set fixed view size (no scaling with window)
    m_gameView.setSize(MAP_PIXEL_WIDTH, MAP_PIXEL_HEIGHT);
    m_gameView.setCenter(MAP_PIXEL_WIDTH / 2.0f, MAP_PIXEL_HEIGHT / 2.0f);

    // Optional: Center on player but keep fixed size
    // m_gameView.setCenter(playerPos);
}
