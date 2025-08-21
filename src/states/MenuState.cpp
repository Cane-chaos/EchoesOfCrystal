#include "states/MenuState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "core/SaveSystem.h"
#include <iostream>

MenuState::MenuState(StateStack& stack, Context context)
    : State(stack, context)
    , m_selectedButton(0)
    , m_showPokemonSelection(false)
    , m_selectedPokemon(0)
{
    setupUI();
}

void MenuState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw background
    window.draw(m_backgroundSprite);
    
    // Draw title
    m_titleLabel.draw(window);
    
    if (!m_showPokemonSelection) {
        // Draw main menu buttons
        for (const auto& button : m_buttons) {
            button.draw(window);
        }
    } else {
        // Draw pokemon selection
        for (const auto& button : m_pokemonButtons) {
            button.draw(window);
        }
    }
}

bool MenuState::update(sf::Time dt) {
    // Update buttons
    for (auto& button : m_buttons) {
        button.update(dt);
    }
    
    for (auto& button : m_pokemonButtons) {
        button.update(dt);
    }
    
    return false; // Don't update states below
}

bool MenuState::handleEvent(const sf::Event& event) {
    if (!m_showPokemonSelection) {
        // Handle main menu
        for (auto& button : m_buttons) {
            if (button.handleEvent(event)) {
                return true;
            }
        }
        
        // Keyboard navigation
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                m_selectedButton = (m_selectedButton - 1 + m_buttons.size()) % m_buttons.size();
                for (size_t i = 0; i < m_buttons.size(); ++i) {
                    m_buttons[i].setSelected(i == m_selectedButton);
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Down) {
                m_selectedButton = (m_selectedButton + 1) % m_buttons.size();
                for (size_t i = 0; i < m_buttons.size(); ++i) {
                    m_buttons[i].setSelected(i == m_selectedButton);
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Enter) {
                m_buttons[m_selectedButton].activate();
                return true;
            }
        }
    } else {
        // Handle pokemon selection
        for (auto& button : m_pokemonButtons) {
            if (button.handleEvent(event)) {
                return true;
            }
        }
        
        // Keyboard navigation for pokemon selection
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                m_selectedPokemon = (m_selectedPokemon - 1 + m_pokemonButtons.size()) % m_pokemonButtons.size();
                for (size_t i = 0; i < m_pokemonButtons.size(); ++i) {
                    m_pokemonButtons[i].setSelected(i == m_selectedPokemon);
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Right) {
                m_selectedPokemon = (m_selectedPokemon + 1) % m_pokemonButtons.size();
                for (size_t i = 0; i < m_pokemonButtons.size(); ++i) {
                    m_pokemonButtons[i].setSelected(i == m_selectedPokemon);
                }
                return true;
            }
            else if (event.key.code == sf::Keyboard::Enter) {
                m_pokemonButtons[m_selectedPokemon].activate();
                return true;
            }
            else if (event.key.code == sf::Keyboard::Escape) {
                m_showPokemonSelection = false;
                return true;
            }
        }
    }
    
    return false;
}

void MenuState::setupUI() {
    AssetManager& assets = *getContext().assets;
    
    // Setup background
    m_backgroundSprite.setTexture(assets.getDefaultTexture());
    m_backgroundSprite.setScale(
        static_cast<float>(Constants::SCREEN_WIDTH),
        static_cast<float>(Constants::SCREEN_HEIGHT)
    );
    m_backgroundSprite.setColor(sf::Color(20, 20, 40));
    
    // Setup title
    m_titleLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 100.0f));
    m_titleLabel.setText("Mini Game SFML");
    m_titleLabel.setFont(assets.getDefaultFont());
    m_titleLabel.setCharacterSize(48);
    m_titleLabel.setAlignment(UI::TextLabel::Alignment::Center);
    
    // Setup main menu buttons
    float buttonWidth = 200.0f;
    float buttonHeight = 50.0f;
    float startY = 300.0f;
    float spacing = 70.0f;
    
    m_buttons.resize(3);
    
    m_buttons[0] = UI::Button(
        sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, startY),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Start Game"
    );
    m_buttons[0].setFont(assets.getDefaultFont());
    m_buttons[0].setCallback([this]() { onStartGame(); });
    
    m_buttons[1] = UI::Button(
        sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, startY + spacing),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Load Game"
    );
    m_buttons[1].setFont(assets.getDefaultFont());
    m_buttons[1].setCallback([this]() { onLoadGame(); });
    
    m_buttons[2] = UI::Button(
        sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, startY + spacing * 2),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Exit"
    );
    m_buttons[2].setFont(assets.getDefaultFont());
    m_buttons[2].setCallback([this]() { onExit(); });
    
    // Set first button as selected
    m_buttons[0].setSelected(true);
    
    // Setup pokemon selection buttons
    m_pokemonButtons.resize(3);
    float pokemonButtonWidth = 150.0f;
    float pokemonStartX = (Constants::SCREEN_WIDTH - (pokemonButtonWidth * 3 + spacing * 2)) / 2.0f;
    
    m_pokemonButtons[0] = UI::Button(
        sf::Vector2f(pokemonStartX, startY),
        sf::Vector2f(pokemonButtonWidth, buttonHeight),
        "Blazeling (Fire)"
    );
    m_pokemonButtons[0].setFont(assets.getDefaultFont());
    m_pokemonButtons[0].setCallback([this]() { 
        // Start game with Fire pokemon
        requestStateClear();
        requestStackPush(StateID::Map);
    });
    
    m_pokemonButtons[1] = UI::Button(
        sf::Vector2f(pokemonStartX + pokemonButtonWidth + spacing, startY),
        sf::Vector2f(pokemonButtonWidth, buttonHeight),
        "Aquary (Water)"
    );
    m_pokemonButtons[1].setFont(assets.getDefaultFont());
    m_pokemonButtons[1].setCallback([this]() { 
        // Start game with Water pokemon
        requestStateClear();
        requestStackPush(StateID::Map);
    });
    
    m_pokemonButtons[2] = UI::Button(
        sf::Vector2f(pokemonStartX + (pokemonButtonWidth + spacing) * 2, startY),
        sf::Vector2f(pokemonButtonWidth, buttonHeight),
        "Verdil (Grass)"
    );
    m_pokemonButtons[2].setFont(assets.getDefaultFont());
    m_pokemonButtons[2].setCallback([this]() { 
        // Start game with Grass pokemon
        requestStateClear();
        requestStackPush(StateID::Map);
    });
    
    m_pokemonButtons[0].setSelected(true);
}

void MenuState::onStartGame() {
    m_showPokemonSelection = true;
}

void MenuState::onLoadGame() {
    SaveSystem& saveSystem = *getContext().save;
    
    if (saveSystem.saveExists()) {
        // Load game and go to map
        requestStateClear();
        requestStackPush(StateID::Map);
    } else {
        std::cout << "No save file found!" << std::endl;
    }
}

void MenuState::onExit() {
    requestStateClear();
}
