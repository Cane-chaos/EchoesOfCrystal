#include "states/ReadyState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "ui/TextLabel.h"
#include <string>

ReadyState::ReadyState(StateStack& stack, Context context)
    : State(stack, context)
    , m_timer(sf::Time::Zero)
    , m_duration(sf::milliseconds(Constants::READY_SCREEN_TIME))
    , m_finished(false)
{
    setupUI();
}

void ReadyState::draw() {
    auto& window = *getContext().window;
    
    // Draw semi-transparent background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(background);
    
    // Draw UI elements
    m_readyLabel.draw(window, sf::RenderStates::Default);
    m_countdownLabel.draw(window, sf::RenderStates::Default);
}

bool ReadyState::update(sf::Time dt) {
    m_timer += dt;
    
    // Update countdown display
    float remainingSeconds = (m_duration - m_timer).asSeconds();
    if (remainingSeconds > 0) {
        int countdown = static_cast<int>(remainingSeconds) + 1;
        m_countdownLabel.setString(std::to_string(countdown));
    } else {
        m_countdownLabel.setString("0");
    }
    
    // Check if ready period is finished
    if (m_timer >= m_duration && !m_finished) {
        finishReady();
    }
    
    return false; // Don't block states below
}

bool ReadyState::handleEvent(const sf::Event& event) {
    // Allow skipping with any key press
    if (event.type == sf::Event::KeyPressed && !m_finished) {
        finishReady();
        return true;
    }
    
    return false;
}

void ReadyState::setReadyCallback(ReadyCallback callback) {
    m_readyCallback = callback;
}

void ReadyState::setDuration(sf::Time duration) {
    m_duration = duration;
}

void ReadyState::setupUI() {
    auto& assets = *getContext().assets;
    
    // Setup ready label
    m_readyLabel.setFont(assets.getDefaultFont());
    m_readyLabel.setString("Are you Ready!?");
    m_readyLabel.setCharacterSize(48);
    m_readyLabel.setFillColor(sf::Color::White);
    m_readyLabel.setStyle(sf::Text::Bold);
    
    // Center the ready label
    sf::FloatRect readyBounds = m_readyLabel.getBounds();
    m_readyLabel.setPosition(
        (Constants::SCREEN_WIDTH - readyBounds.width) / 2.0f,
        Constants::SCREEN_HEIGHT / 2.0f - 50.0f
    );
    
    // Setup countdown label
    m_countdownLabel.setFont(assets.getDefaultFont());
    m_countdownLabel.setString("3");
    m_countdownLabel.setCharacterSize(72);
    m_countdownLabel.setFillColor(sf::Color::Yellow);
    m_countdownLabel.setStyle(sf::Text::Bold);
    
    // Center the countdown label
    sf::FloatRect countdownBounds = m_countdownLabel.getBounds();
    m_countdownLabel.setPosition(
        (Constants::SCREEN_WIDTH - countdownBounds.width) / 2.0f,
        Constants::SCREEN_HEIGHT / 2.0f + 20.0f
    );
}

void ReadyState::finishReady() {
    if (m_finished) return;
    
    m_finished = true;
    
    // Call the callback if set
    if (m_readyCallback) {
        m_readyCallback();
    }
    
    // Remove this state
    requestStackPop();
}
