#include "states/MapState.h"
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

MapState::MapState(StateStack& stack, Context context)
    : State(stack, context)
    , m_canRoll(true)
{
    setupUI();
    
    // Initialize new game by default
    initializeNewGame(PokemonType::Fire);
}

void MapState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Set the game view
    window.setView(m_gameView);
    
    // Draw map
    m_map.draw(window, m_gameView);
    
    // Draw player
    m_player.draw(window, sf::RenderStates::Default);
    
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
    m_player.update(dt);
    m_rollButton.update(dt);
    m_hpBar.update(dt);
    m_mpBar.update(dt);
    
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
    }
    
    return false;
}

void MapState::initializeNewGame(PokemonType pokemonType) {
    // TODO: Use pokemonType to create appropriate Pokemon
    (void)pokemonType; // Suppress unused parameter warning

    // Generate new map
    m_map.generateMap(*getContext().rng);
    
    // Initialize player
    m_player.setMapPosition(m_map.getStartPosition());
    m_player.setDirection(Direction::Up);
    
    // Set player position in world coordinates
    Vec2i mapPos = m_player.getMapPosition();
    m_player.setPosition(
        mapPos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f,
        mapPos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2.0f
    );
    
    m_canRoll = true;
}

void MapState::loadGame(const SaveData& saveData) {
    // Load map from save data
    // For now, generate a new map with the saved seed
    getContext().rng->setSeed(saveData.mapSeed);
    m_map.generateMap(*getContext().rng);
    
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
    if (!m_canRoll) return;
    
    m_canRoll = false;
    requestStackPush(StateID::Dice);
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
        case TileType::Pit:
            requestStackPush(StateID::Coin);
            break;
        case TileType::Rift:
            requestStackPush(StateID::Dice);
            break;
        case TileType::ArrowUp:
        case TileType::ArrowDown:
        case TileType::ArrowLeft:
        case TileType::ArrowRight:
            // Change player direction
            // This would be implemented based on arrow type
            break;
        case TileType::Enemy:
            requestStackPush(StateID::Combat);
            break;
        case TileType::Boss:
            requestStackPush(StateID::Combat);
            break;
        default:
            break;
    }
    
    m_canRoll = true;
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
    m_gameView.setCenter(playerPos);
}
