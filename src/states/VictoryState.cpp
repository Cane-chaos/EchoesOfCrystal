#include "states/VictoryState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"

VictoryState::VictoryState(StateStack& stack, Context context)
    : State(stack, context)
{
    setupUI();
}

void VictoryState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(20, 40, 20));
    window.draw(background);
    
    m_titleLabel.draw(window);
    m_messageLabel.draw(window);
    m_exitButton.draw(window);
}

bool VictoryState::update(sf::Time dt) {
    m_exitButton.update(dt);
    return false;
}

bool VictoryState::handleEvent(const sf::Event& event) {
    if (m_exitButton.handleEvent(event)) {
        return true;
    }
    
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape) {
            onExit();
            return true;
        }
    }
    
    return false;
}

void VictoryState::setupUI() {
    AssetManager& assets = *getContext().assets;
    
    m_titleLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 200));
    m_titleLabel.setFont(assets.getDefaultFont());
    m_titleLabel.setCharacterSize(48);
    m_titleLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_titleLabel.setText("VICTORY!");
    m_titleLabel.setFillColor(sf::Color::Green);
    
    m_messageLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 280));
    m_messageLabel.setFont(assets.getDefaultFont());
    m_messageLabel.setCharacterSize(24);
    m_messageLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_messageLabel.setText("Congratulations! You have defeated the boss!");
    
    float buttonWidth = 150;
    float buttonHeight = 50;
    
    m_exitButton.setPosition(sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, 400));
    m_exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_exitButton.setText("Exit");
    m_exitButton.setFont(assets.getDefaultFont());
    m_exitButton.setCallback([this]() { onExit(); });
    m_exitButton.setSelected(true);
}

void VictoryState::onExit() {
    requestStateClear();
}
