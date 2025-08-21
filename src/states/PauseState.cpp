#include "states/PauseState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "core/SaveSystem.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"

PauseState::PauseState(StateStack& stack, Context context)
    : State(stack, context)
    , m_selectedButton(0)
{
    setupUI();
}

void PauseState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw semi-transparent background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(background);
    
    m_titleLabel.draw(window);
    m_resumeButton.draw(window);
    m_saveButton.draw(window);
    m_mainMenuButton.draw(window);
}

bool PauseState::update(sf::Time dt) {
    m_resumeButton.update(dt);
    m_saveButton.update(dt);
    m_mainMenuButton.update(dt);
    return false;
}

bool PauseState::handleEvent(const sf::Event& event) {
    if (m_resumeButton.handleEvent(event) || 
        m_saveButton.handleEvent(event) || 
        m_mainMenuButton.handleEvent(event)) {
        return true;
    }
    
    // Keyboard navigation
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            m_selectedButton = (m_selectedButton - 1 + 3) % 3;
            updateButtonSelection();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Down) {
            m_selectedButton = (m_selectedButton + 1) % 3;
            updateButtonSelection();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Enter) {
            switch (m_selectedButton) {
                case 0: onResume(); break;
                case 1: onSave(); break;
                case 2: onMainMenu(); break;
            }
            return true;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            onResume();
            return true;
        }
    }
    
    return false;
}

void PauseState::setupUI() {
    AssetManager& assets = *getContext().assets;
    
    m_titleLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 200));
    m_titleLabel.setFont(assets.getDefaultFont());
    m_titleLabel.setCharacterSize(36);
    m_titleLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_titleLabel.setText("PAUSED");
    
    float buttonWidth = 150;
    float buttonHeight = 50;
    float spacing = 70;
    float startY = 300;
    
    m_resumeButton.setPosition(sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, startY));
    m_resumeButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_resumeButton.setText("Resume");
    m_resumeButton.setFont(assets.getDefaultFont());
    m_resumeButton.setCallback([this]() { onResume(); });
    m_resumeButton.setSelected(true);
    
    m_saveButton.setPosition(sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, startY + spacing));
    m_saveButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_saveButton.setText("Save Game");
    m_saveButton.setFont(assets.getDefaultFont());
    m_saveButton.setCallback([this]() { onSave(); });
    
    m_mainMenuButton.setPosition(sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, startY + spacing * 2));
    m_mainMenuButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_mainMenuButton.setText("Main Menu");
    m_mainMenuButton.setFont(assets.getDefaultFont());
    m_mainMenuButton.setCallback([this]() { onMainMenu(); });
}

void PauseState::updateButtonSelection() {
    m_resumeButton.setSelected(m_selectedButton == 0);
    m_saveButton.setSelected(m_selectedButton == 1);
    m_mainMenuButton.setSelected(m_selectedButton == 2);
}

void PauseState::onResume() {
    requestStackPop();
}

void PauseState::onSave() {
    // Save game functionality would be implemented here
    // For now, just show a message and resume
    requestStackPop();
}

void PauseState::onMainMenu() {
    requestStateClear();
    requestStackPush(StateID::Menu);
}
