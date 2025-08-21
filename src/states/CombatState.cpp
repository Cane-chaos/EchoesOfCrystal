#include "states/CombatState.h"
#include "StateStack.h"
#include "entities/Enemy.h"
#include "entities/Boss.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "core/RNG.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include "ui/Panel.h"
#include <SFML/Graphics.hpp>

CombatState::CombatState(StateStack& stack, Context context)
    : State(stack, context)
    , m_player(nullptr)
    , m_isBoss(false)
    , m_phase(CombatPhase::PlayerCoin)
    , m_skillSelectionVisible(false)
    , m_waitingForInput(false)
{
    setupUI();
}

void CombatState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Draw background
    sf::RectangleShape background(sf::Vector2f(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    background.setFillColor(sf::Color(20, 20, 40));
    window.draw(background);
    
    // Draw panels
    m_playerPanel.draw(window);
    m_enemyPanel.draw(window);
    m_logPanel.draw(window);
    
    // Draw labels and bars
    m_playerNameLabel.draw(window);
    m_enemyNameLabel.draw(window);
    m_playerHPBar.draw(window);
    m_playerMPBar.draw(window);
    m_enemyHPBar.draw(window);
    m_enemyMPBar.draw(window);
    
    // Draw log messages
    for (const auto& message : m_logMessages) {
        message.draw(window);
    }
    
    // Draw action buttons
    if (m_phase == CombatPhase::PlayerAction) {
        m_normalAttackButton.draw(window);
        
        if (m_skillSelectionVisible) {
            m_skillPanel.draw(window);
            for (const auto& button : m_skillButtons) {
                button.draw(window);
            }
        }
    }
}

bool CombatState::update(sf::Time dt) {
    m_playerHPBar.update(dt);
    m_playerMPBar.update(dt);
    m_enemyHPBar.update(dt);
    m_enemyMPBar.update(dt);
    
    m_normalAttackButton.update(dt);
    for (auto& button : m_skillButtons) {
        button.update(dt);
    }
    
    updateUI();
    
    return false;
}

bool CombatState::handleEvent(const sf::Event& event) {
    if (m_phase == CombatPhase::PlayerAction && m_waitingForInput) {
        if (m_normalAttackButton.handleEvent(event)) {
            return true;
        }
        
        if (m_skillSelectionVisible) {
            for (auto& button : m_skillButtons) {
                if (button.handleEvent(event)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void CombatState::initializeCombat(Player& player, bool isBoss) {
    m_player = &player;
    m_isBoss = isBoss;
    
    if (isBoss) {
        m_enemy = Boss::createBoss();
    } else {
        m_enemy = Enemy::createRegularEnemy();
    }
    
    addLogMessage("Combat begins!");
    startPlayerTurn();
}

void CombatState::setupUI() {
    AssetManager& assets = *getContext().assets;
    
    // Setup panels
    m_playerPanel.setPosition(sf::Vector2f(50, 400));
    m_playerPanel.setSize(sf::Vector2f(300, 200));
    
    m_enemyPanel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 350, 100));
    m_enemyPanel.setSize(sf::Vector2f(300, 200));
    
    m_logPanel.setPosition(sf::Vector2f(50, 50));
    m_logPanel.setSize(sf::Vector2f(Constants::SCREEN_WIDTH - 100, 150));
    
    m_skillPanel.setPosition(sf::Vector2f(400, 400));
    m_skillPanel.setSize(sf::Vector2f(400, 200));
    
    // Setup labels
    m_playerNameLabel.setPosition(sf::Vector2f(60, 410));
    m_playerNameLabel.setFont(assets.getDefaultFont());
    m_playerNameLabel.setCharacterSize(18);
    m_playerNameLabel.setText("Player");
    
    m_enemyNameLabel.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 340, 110));
    m_enemyNameLabel.setFont(assets.getDefaultFont());
    m_enemyNameLabel.setCharacterSize(18);
    m_enemyNameLabel.setText("Enemy");
    
    // Setup bars
    m_playerHPBar.setPosition(sf::Vector2f(60, 440));
    m_playerHPBar.setSize(sf::Vector2f(200, 20));
    m_playerHPBar.setFont(assets.getDefaultFont());
    m_playerHPBar.setFillColor(sf::Color::Red);
    
    m_playerMPBar.setPosition(sf::Vector2f(60, 470));
    m_playerMPBar.setSize(sf::Vector2f(200, 20));
    m_playerMPBar.setFont(assets.getDefaultFont());
    m_playerMPBar.setFillColor(sf::Color::Blue);
    
    m_enemyHPBar.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 340, 140));
    m_enemyHPBar.setSize(sf::Vector2f(200, 20));
    m_enemyHPBar.setFont(assets.getDefaultFont());
    m_enemyHPBar.setFillColor(sf::Color::Red);
    
    m_enemyMPBar.setPosition(sf::Vector2f(Constants::SCREEN_WIDTH - 340, 170));
    m_enemyMPBar.setSize(sf::Vector2f(200, 20));
    m_enemyMPBar.setFont(assets.getDefaultFont());
    m_enemyMPBar.setFillColor(sf::Color::Blue);
    
    // Setup buttons
    m_normalAttackButton.setPosition(sf::Vector2f(60, 520));
    m_normalAttackButton.setSize(sf::Vector2f(120, 40));
    m_normalAttackButton.setText("Attack");
    m_normalAttackButton.setFont(assets.getDefaultFont());
    m_normalAttackButton.setCallback([this]() { onNormalAttackSelected(); });
}

void CombatState::updateUI() {
    if (m_player) {
        m_playerHPBar.setValues(m_player->getCurrentHP(), m_player->getMaxHP());
        m_playerMPBar.setValues(m_player->getCurrentMP(), m_player->getMaxMP());
    }
    
    if (m_enemy) {
        m_enemyHPBar.setValues(m_enemy->getCurrentHP(), m_enemy->getMaxHP());
        m_enemyMPBar.setValues(m_enemy->getCurrentMP(), m_enemy->getMaxMP());
        m_enemyNameLabel.setText(m_enemy->getName());
    }
}

void CombatState::addLogMessage(const std::string& message) {
    // Keep only last 5 messages
    if (m_logMessages.size() >= 5) {
        m_logMessages.erase(m_logMessages.begin());
    }
    
    UI::TextLabel label;
    label.setPosition(sf::Vector2f(60, 60 + m_logMessages.size() * 25));
    label.setFont(getContext().assets->getDefaultFont());
    label.setCharacterSize(14);
    label.setText(message);
    
    m_logMessages.push_back(label);
    
    // Reposition all messages
    for (size_t i = 0; i < m_logMessages.size(); ++i) {
        m_logMessages[i].setPosition(sf::Vector2f(60, 60 + i * 25));
    }
}

void CombatState::startPlayerTurn() {
    m_phase = CombatPhase::PlayerCoin;
    performCoinFlip();
}

void CombatState::startEnemyTurn() {
    m_phase = CombatPhase::EnemyAction;
    performEnemyAction();
}

void CombatState::performCoinFlip() {
    // For now, just randomly decide if player gets skills
    bool coinResult = getContext().rng->flipCoin();
    onCoinResult(coinResult);
}

void CombatState::onCoinResult(bool playerWon) {
    if (playerWon) {
        addLogMessage("Coin flip: SUCCESS! You can use skills.");
        showSkillSelection();
    } else {
        addLogMessage("Coin flip: FAILED! Normal attack only.");
        hideSkillSelection();
    }
    
    m_phase = CombatPhase::PlayerAction;
    m_waitingForInput = true;
}

void CombatState::showSkillSelection() {
    m_skillSelectionVisible = true;
    // Setup skill buttons based on available skills
    // This is simplified for now
}

void CombatState::hideSkillSelection() {
    m_skillSelectionVisible = false;
}

void CombatState::useSkill(SkillType skillType) {
    if (m_player && m_player->getPokemon() && m_enemy) {
        m_player->getPokemon()->useSkill(skillType, *m_enemy);
        addLogMessage("Player used skill!");
        m_waitingForInput = false;
        checkCombatEnd();
        if (m_phase != CombatPhase::Ended) {
            startEnemyTurn();
        }
    }
}

void CombatState::performNormalAttack() {
    if (m_player && m_player->getPokemon() && m_enemy) {
        m_player->getPokemon()->normalAttack(*m_enemy);
        addLogMessage("Player attacks!");
        m_waitingForInput = false;
        checkCombatEnd();
        if (m_phase != CombatPhase::Ended) {
            startEnemyTurn();
        }
    }
}

void CombatState::performEnemyAction() {
    if (m_enemy && m_player) {
        CombatAction action = m_enemy->chooseAction(*m_player);
        m_enemy->performAction(action, *m_player);
        addLogMessage("Enemy attacks!");
        
        checkCombatEnd();
        if (m_phase != CombatPhase::Ended) {
            startPlayerTurn();
        }
    }
}

void CombatState::checkCombatEnd() {
    if (m_player && m_player->isDead()) {
        endCombat(CombatResult::EnemyWin);
    } else if (m_enemy && m_enemy->isDead()) {
        endCombat(CombatResult::PlayerWin);
    }
}

void CombatState::endCombat(CombatResult result) {
    m_phase = CombatPhase::Ended;
    
    switch (result) {
        case CombatResult::PlayerWin:
            if (m_isBoss) {
                addLogMessage("Boss defeated! Victory!");
                requestStateClear();
                requestStackPush(StateID::Victory);
            } else {
                addLogMessage("Enemy defeated!");
                if (m_player) {
                    m_player->addVictory();
                }
                requestStackPop(); // Return to map
            }
            break;
        case CombatResult::EnemyWin:
            addLogMessage("You were defeated!");
            requestStateClear();
            requestStackPush(StateID::GameOver);
            break;
        default:
            break;
    }
}

void CombatState::onNormalAttackSelected() {
    performNormalAttack();
}

void CombatState::onSkillSelected(SkillType skillType) {
    useSkill(skillType);
}
