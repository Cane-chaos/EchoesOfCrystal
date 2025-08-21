#include "states/CoinState.h"
#include "StateStack.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "core/AudioManager.h"
#include "core/RNG.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include <string>

CoinState::CoinState(StateStack& stack, Context context)
    : State(stack, context)
    , m_phase(Phase::Selection)
    , m_animationTime(sf::Time::Zero)
    , m_totalAnimationTime(sf::milliseconds(Constants::COIN_ANIMATION_TIME))
    , m_playerChoice(CoinChoice::Head)
    , m_coinResult(true)
    , m_playerWon(false)
{
    setupUI();
}

void CoinState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw semi-transparent background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(background);
    
    m_instructionLabel.draw(window);
    m_coinLabel.draw(window);
    
    if (m_phase == Phase::Selection) {
        m_headButton.draw(window);
        m_tailButton.draw(window);
    } else if (m_phase == Phase::Result) {
        m_resultLabel.draw(window);
    }
}

bool CoinState::update(sf::Time dt) {
    m_headButton.update(dt);
    m_tailButton.update(dt);
    
    if (m_phase == Phase::Flipping) {
        m_animationTime += dt;
        
        // Animate coin flip
        if (m_animationTime < m_totalAnimationTime) {
            // Alternate between H and T rapidly
            bool showHeads = (static_cast<int>(m_animationTime.asMilliseconds() / 100) % 2) == 0;
            m_coinLabel.setText(showHeads ? "H" : "T");
        } else {
            finishAnimation();
        }
    }
    
    return false;
}

bool CoinState::handleEvent(const sf::Event& event) {
    if (m_phase == Phase::Selection) {
        if (m_headButton.handleEvent(event) || m_tailButton.handleEvent(event)) {
            return true;
        }
        
        // Keyboard selection
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                m_headButton.setSelected(true);
                m_tailButton.setSelected(false);
                return true;
            } else if (event.key.code == sf::Keyboard::Right) {
                m_headButton.setSelected(false);
                m_tailButton.setSelected(true);
                return true;
            } else if (event.key.code == sf::Keyboard::Enter) {
                if (m_headButton.isSelected()) {
                    onHeadSelected();
                } else {
                    onTailSelected();
                }
                return true;
            }
        }
    } else if (m_phase == Phase::Result) {
        if (event.type == sf::Event::KeyPressed || 
            event.type == sf::Event::MouseButtonPressed) {
            
            if (m_resultCallback) {
                m_resultCallback(m_playerWon);
            }
            requestStackPop();
            return true;
        }
    }
    
    return false;
}

void CoinState::setResultCallback(ResultCallback callback) {
    m_resultCallback = callback;
}

void CoinState::setupUI() {
    AssetManager& assets = *getContext().assets;
    
    m_instructionLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, 200));
    m_instructionLabel.setFont(assets.getDefaultFont());
    m_instructionLabel.setCharacterSize(24);
    m_instructionLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_instructionLabel.setText("Choose your guess:");
    
    m_coinLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, Constants::SCREEN_HEIGHT / 2.0f));
    m_coinLabel.setFont(assets.getDefaultFont());
    m_coinLabel.setCharacterSize(96);
    m_coinLabel.setAlignment(UI::TextLabel::Alignment::Center);
    m_coinLabel.setText("?");
    
    m_resultLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f, Constants::SCREEN_HEIGHT / 2.0f + 100));
    m_resultLabel.setFont(assets.getDefaultFont());
    m_resultLabel.setCharacterSize(24);
    m_resultLabel.setAlignment(UI::TextLabel::Alignment::Center);
    
    float buttonWidth = 120;
    float buttonHeight = 50;
    float spacing = 50;
    
    m_headButton.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f - buttonWidth - spacing / 2, 350));
    m_headButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_headButton.setText("HEADS");
    m_headButton.setFont(assets.getDefaultFont());
    m_headButton.setCallback([this]() { onHeadSelected(); });
    m_headButton.setSelected(true);
    
    m_tailButton.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH / 2.0f + spacing / 2, 350));
    m_tailButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_tailButton.setText("TAILS");
    m_tailButton.setFont(assets.getDefaultFont());
    m_tailButton.setCallback([this]() { onTailSelected(); });
}

void CoinState::onHeadSelected() {
    m_playerChoice = CoinChoice::Head;
    startFlipAnimation();
}

void CoinState::onTailSelected() {
    m_playerChoice = CoinChoice::Tail;
    startFlipAnimation();
}

void CoinState::startFlipAnimation() {
    m_phase = Phase::Flipping;
    m_animationTime = sf::Time::Zero;
    m_coinResult = getContext().rng->flipCoin(); // true = heads, false = tails
    
    m_instructionLabel.setText("Flipping coin...");
    
    // Play coin sound
    getContext().audio->playSound(Constants::SFX_COIN);
}

void CoinState::finishAnimation() {
    m_phase = Phase::Result;
    
    // Show final result
    m_coinLabel.setText(m_coinResult ? "H" : "T");
    
    // Check if player won
    m_playerWon = (m_playerChoice == CoinChoice::Head && m_coinResult) ||
                  (m_playerChoice == CoinChoice::Tail && !m_coinResult);
    
    std::string resultText = m_coinResult ? "HEADS" : "TAILS";
    if (m_playerWon) {
        m_resultLabel.setText("Result: " + resultText + " - You WIN! Press any key to continue");
        m_resultLabel.setFillColor(sf::Color::Green);
    } else {
        m_resultLabel.setText("Result: " + resultText + " - You LOSE! Press any key to continue");
        m_resultLabel.setFillColor(sf::Color::Red);
    }
}
