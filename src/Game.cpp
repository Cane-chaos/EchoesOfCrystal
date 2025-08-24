#include "Game.h"
#include "states/MenuState.h"
#include "states/MapState.h"
#include "states/DiceState.h"
#include "states/CoinState.h"
#include "states/CombatState.h"
#include "states/GameOverState.h"
#include "states/VictoryState.h"
#include "states/PauseState.h"
#include <iostream>

const sf::Time Game::TimePerFrame = sf::seconds(1.f / Constants::TARGET_FPS);
Game* g_game = nullptr;

Game::Game()
    : m_window(sf::VideoMode(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT), "Mini Game SFML")
    , m_stateStack(State::Context(m_window, m_audioManager, m_assetManager, m_rng, m_saveSystem))
    , m_statisticsUpdateTime()
    , m_statisticsNumFrames(0)
    , m_isPaused(false)
{
    g_game = this;
    
    m_window.setKeyRepeatEnabled(false);
    m_window.setVerticalSyncEnabled(true);
    
    // Initialize statistics text
    m_statisticsText.setFont(m_assetManager.getDefaultFont());
    m_statisticsText.setPosition(5.f, 5.f);
    m_statisticsText.setCharacterSize(12);
    m_statisticsText.setFillColor(sf::Color::White);
    
    // Load all game assets
    m_assetManager.loadAll();

    registerStates();
    m_stateStack.pushState(StateID::Menu);
}

Game::~Game() {
    g_game = nullptr;
}

void Game::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    
    while (m_window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        timeSinceLastUpdate += deltaTime;
        
        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            
            processInput();
            update(TimePerFrame);
            
            if (m_stateStack.isEmpty()) {
                m_window.close();
            }
        }
        
        updateStatistics(deltaTime);
        render();
    }
}

void Game::processInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        m_stateStack.handleEvent(event);
        
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
    }
}

void Game::update(sf::Time deltaTime) {
    m_stateStack.update(deltaTime);
}

void Game::render() {
    m_window.clear();
    m_stateStack.draw();
    
#ifdef DEBUG
    m_window.draw(m_statisticsText);
#endif
    
    m_window.display();
}

void Game::registerStates() {
    m_stateStack.registerState<MenuState>(StateID::Menu);
    m_stateStack.registerState<MapState>(StateID::Map);
    m_stateStack.registerState<DiceState>(StateID::Dice);
    m_stateStack.registerState<CoinState>(StateID::Coin);
    m_stateStack.registerState<CombatState>(StateID::Combat);
    m_stateStack.registerState<GameOverState>(StateID::GameOver);
    m_stateStack.registerState<VictoryState>(StateID::Victory);
    m_stateStack.registerState<PauseState>(StateID::Pause);
}

void Game::updateStatistics(sf::Time deltaTime) {
    m_statisticsUpdateTime += deltaTime;
    m_statisticsNumFrames += 1;
    
    if (m_statisticsUpdateTime >= sf::seconds(1.0f)) {
        m_statisticsText.setString("FPS: " + std::to_string(m_statisticsNumFrames));
        
        m_statisticsUpdateTime -= sf::seconds(1.0f);
        m_statisticsNumFrames = 0;
    }
}

void Game::requestStackPush(StateID stateID) {
    m_stateStack.pushState(stateID);
}

void Game::requestStackPop() {
    m_stateStack.popState();
}

void Game::requestStackClear() {
    m_stateStack.clearStates();
}

void Game::requestStateClear() {
    m_stateStack.clearStates();
}
