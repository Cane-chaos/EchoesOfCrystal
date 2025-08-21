#include "states/GameOverState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"

GameOverState::GameOverState(StateStack& stack, Context context)
    : State(stack, context)
    , m_selectedButton(0)
{
    setupUI();
}

void GameOverState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(40, 20, 20));
    window.draw(background);
    
    m_titleLabel.draw(window);
    m_messageLabel.draw(window);
    m_retryButton.draw(window);
    m_exitButton.draw(window);
}

bool GameOverState::update(sf::Time dt) {
    m_retryButton.update(dt);
    m_exitButton.update(dt);
    return false;
}

bool GameOverState::handleEvent(const sf::Event& event) {
    if (m_retryButton.handleEvent(event) || m_exitButton.handleEvent(event)) {
        return true;
    }
    
    // Keyboard navigation
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
            m_selectedButton = 1 - m_selectedButton; // Toggle between 0 and 1
            m_retryButton.setSelected(m_selectedButton == 0);
            m_exitButton.setSelected(m_selectedButton == 1);
            return true;
        }
        else if (event.key.code == sf::Keyboard::Enter) {
            if (m_selectedButton == 0) {
                onRetry();
            } else {
                onExit();
            }
            return true;
        }
    }
    
    return false;
}

void GameOverState::setupUI() {
    AssetManager& assets = *getContext().assets;
    
    m_titleLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 200));
    m_titleLabel.setFont(assets.getDefaultFont());
    m_titleLabel.setCharacterSize(48);
    m_titleLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_titleLabel.setText("GAME OVER");
    m_titleLabel.setFillColor(sf::Color::Red);
    
    m_messageLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 280));
    m_messageLabel.setFont(assets.getDefaultFont());
    m_messageLabel.setCharacterSize(24);
    m_messageLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_messageLabel.setText("You have been defeated!");
    
    float buttonWidth = 150;
    float buttonHeight = 50;
    float spacing = 70;
    
    m_retryButton.setPosition(sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, 400));
    m_retryButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_retryButton.setText("Retry");
    m_retryButton.setFont(assets.getDefaultFont());
    m_retryButton.setCallback([this]() { onRetry(); });
    m_retryButton.setSelected(true);
    
    m_exitButton.setPosition(sf::Vector2f((Constants::SCREEN_WIDTH - buttonWidth) / 2.0f, 400 + spacing));
    m_exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_exitButton.setText("Exit");
    m_exitButton.setFont(assets.getDefaultFont());
    m_exitButton.setCallback([this]() { onExit(); });
}

void GameOverState::onRetry() {
    requestStateClear();
    requestStackPush(StateID::Menu);
}

void GameOverState::onExit() {
    requestStateClear();
}
