#include "states/DiceState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/RNG.h"
#include "ui/TextLabel.h"
#include <string>

DiceState::DiceState(StateStack& stack, Context context)
    : State(stack, context)
    , m_animationTime(sf::Time::Zero)
    , m_totalAnimationTime(sf::milliseconds(Constants::DICE_ANIMATION_TIME))
    , m_animating(false)
    , m_finalResult(1)
{
    AssetManager& assets = *getContext().assets;
    
    m_diceLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, Constants::SCREEN_HEIGHT / 2.0f));
    m_diceLabel.setFont(assets.getDefaultFont());
    m_diceLabel.setCharacterSize(72);
    m_diceLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_diceLabel.setText("1");
    
    m_instructionLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, Constants::SCREEN_HEIGHT / 2.0f + 100));
    m_instructionLabel.setFont(assets.getDefaultFont());
    m_instructionLabel.setCharacterSize(24);
    m_instructionLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_instructionLabel.setText("Rolling dice...");
    
    startAnimation();
}

void DiceState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw semi-transparent background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(background);
    
    m_diceLabel.draw(window);
    m_instructionLabel.draw(window);
}

bool DiceState::update(sf::Time dt) {
    if (m_animating) {
        m_animationTime += dt;
        
        // Update dice display with random numbers
        if (m_animationTime < m_totalAnimationTime) {
            int randomNum = getContext().rng->rollD6();
            m_diceLabel.setText(std::to_string(randomNum));
        } else {
            finishAnimation();
        }
    }
    
    return false; // Don't update states below
}

bool DiceState::handleEvent(const sf::Event& event) {
    if (!m_animating) {
        if (event.type == sf::Event::KeyPressed || 
            event.type == sf::Event::MouseButtonPressed) {
            
            if (m_resultCallback) {
                m_resultCallback(m_finalResult);
            }
            requestStackPop();
            return true;
        }
    }
    
    return false;
}

void DiceState::setResultCallback(ResultCallback callback) {
    m_resultCallback = callback;
}

void DiceState::startAnimation() {
    m_animating = true;
    m_animationTime = sf::Time::Zero;
    m_finalResult = getContext().rng->rollD6();
    
    // Play dice sound
    getContext().audio->playSound(Constants::SFX_DICE);
}

void DiceState::finishAnimation() {
    m_animating = false;
    m_diceLabel.setText(std::to_string(m_finalResult));
    m_instructionLabel.setText("Result: " + std::to_string(m_finalResult) + " - Press any key to continue");
}
