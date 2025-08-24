#pragma once
#include "State.h"
#include "world/Map.h"
#include "entities/Player.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include <optional>

class MapState : public State {
public:
    MapState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    // Game state management
    void initializeNewGame(PokemonType pokemonType);
    void loadGame(const SaveData& saveData);
    void saveGame();
    
    // Movement and events
    void rollDice();
    void movePlayer(int steps);
    void handleTileEvent(TileType tileType);
    void handleRockObstacle();
    void handlePortalTeleport();
    void handleGoalReached();

    // Auto-path system
    void startAutoPath(int steps);
    void updateAutoPath(sf::Time dt);
    std::optional<Vec2i> pickNextPosition();
    void checkCombatTrigger();
    void checkGoalReached();

    // Combat result handling
    void onCombatEnded(CombatResult result);
    
    // Getters for other states
    Player& getPlayer() { return m_player; }
    Map& getMap() { return m_map; }
    
private:
    void setupUI();
    void updateUI();
    void centerCameraOnPlayer();
    
private:
    Map m_map;
    Player m_player;
    sf::View m_gameView;
    
    // UI elements
    UI::TextLabel m_positionLabel;
    UI::TextLabel m_directionLabel;
    UI::TextLabel m_levelLabel;
    UI::TextLabel m_victoriesLabel;
    UI::Button m_rollButton;
    UI::Bar m_hpBar;
    UI::Bar m_mpBar;
    
    bool m_canRoll;

    // Auto-path system
    int m_remainingSteps;
    bool m_autoPathActive;
    sf::Time m_stepTimer;
    sf::Time m_stepDelay;
    bool m_justTeleported;
    Vec2i m_combatMonsterPos; // Store monster position for combat result handling
};
