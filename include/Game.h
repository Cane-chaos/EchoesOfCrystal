#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "StateStack.h"
#include "core/AudioManager.h"
#include "core/AssetManager.h"
#include "core/RNG.h"
#include "core/SaveSystem.h"
#include "Constants.h"

class Game {
public:
    Game();
    ~Game();
    
    void run();
    
    // Getters for global systems
    AudioManager& getAudioManager() { return m_audioManager; }
    AssetManager& getAssetManager() { return m_assetManager; }
    RNG& getRNG() { return m_rng; }
    SaveSystem& getSaveSystem() { return m_saveSystem; }
    sf::RenderWindow& getWindow() { return m_window; }
    
    // Game state management
    void requestStackPush(StateID stateID);
    void requestStackPop();
    void requestStackClear();
    void requestStateClear();
    
private:
    void processInput();
    void update(sf::Time deltaTime);
    void render();
    
    void registerStates();
    void updateStatistics(sf::Time deltaTime);
    
private:
    sf::RenderWindow m_window;
    StateStack m_stateStack;
    
    // Global systems
    AudioManager m_audioManager;
    AssetManager m_assetManager;
    RNG m_rng;
    SaveSystem m_saveSystem;
    
    // Performance tracking
    sf::Time m_statisticsUpdateTime;
    std::size_t m_statisticsNumFrames;
    sf::Text m_statisticsText;
    
    // Game state
    bool m_isPaused;
    
    static const sf::Time TimePerFrame;
};

// Global game instance access
extern Game* g_game;
