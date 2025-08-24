#include "states/CombatState.h"
#include "StateStack.h"
#include "entities/Enemy.h"
#include "entities/Boss.h"
#include "Constants.h"
#include "core/AssetManager.h"
#include "core/RNG.h"
#include <iostream>

// Static variable definitions
CombatResult CombatState::s_lastCombatResult = CombatResult::None;
bool CombatState::s_combatJustEnded = false;
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include "ui/Panel.h"
#include <SFML/Graphics.hpp>

CombatState::CombatState(StateStack& stack, Context context)
    : State(stack, context)
    , m_player(nullptr)
    , m_isBoss(false)
    , m_phase(CombatPhase::ReadyBanner)
    , m_skillSelectionVisible(false)
    , m_waitingForInput(false)
    , m_enemyType("chalamander")
    , m_bannerTimer(sf::Time::Zero)
    , m_bannerDuration(sf::seconds(3.0f))
    , m_playerChoice(CoinChoice::None)
    , m_coinResult(CoinResult::None)
    , m_coinTimer(sf::Time::Zero)
    , m_coinDuration(sf::seconds(3.0f))
    , m_coinCorrect(false)
    , m_playerHP(100), m_playerMaxHP(100), m_playerATK(15), m_playerDEF(10)
    , m_enemyHP(80), m_enemyMaxHP(80), m_enemyATK(12), m_enemyDEF(8)
    , m_playerName("Pikachu"), m_enemyName("Enemy")
    , m_resultTimer(sf::Time::Zero)
    , m_resultDuration(sf::seconds(1.5f))
    , m_combatResult(CombatResult::None)
    , m_showingSkillMenu(false)
    , m_isDefenseSkillMenu(false)
{
    setupUI();
}

void CombatState::draw() {
    sf::RenderWindow& window = *getContext().window;
    AssetManager& assets = *getContext().assets;

    // Draw combat background
    sf::Sprite background = assets.makeFullScreenBackground("bg_combat", window.getSize());
    window.draw(background);

    // Draw sprites with proper positioning
    drawCombatSprites(window, assets);
    
    // Remove all old UI panels - only keep compact stat panels
    
    // Draw enhanced UI based on phase
    const sf::Font& font = assets.hasGameFont("arial") ? assets.getGameFont("arial") : assets.getDefaultFont();
    sf::Vector2f windowSize(window.getSize());

    switch (m_phase) {
        case CombatPhase::ReadyBanner:
            CombatUI::drawBanner(window, "Are you Ready!?", font, windowSize);
            break;

        case CombatPhase::PlayerCoinChoice:
            CombatUI::drawCoinChoice(window, font, windowSize);
            m_headButton.draw(window);
            m_tailButton.draw(window);
            break;

        case CombatPhase::PlayerCoinFlip:
            CombatUI::drawCoinFlipping(window, font, windowSize);
            break;

        case CombatPhase::PlayerAction:
            CombatUI::drawCoinResult(window, font, windowSize,
                                   m_coinResult == CoinResult::Head, m_coinCorrect);
            // Show attack prompt
            {
                sf::Text attackText;
                attackText.setFont(font);
                attackText.setString("Press SPACE to attack!");
                attackText.setCharacterSize(24);
                attackText.setFillColor(sf::Color::Yellow);
                sf::FloatRect bounds = attackText.getLocalBounds();
                attackText.setPosition((windowSize.x - bounds.width) / 2.0f, windowSize.y - 100);
                window.draw(attackText);
            }
            break;

        case CombatPhase::Victory:
            CombatUI::drawBanner(window, "Victory!", font, windowSize, sf::Color::Green);
            break;

        case CombatPhase::Defeat:
            CombatUI::drawBanner(window, "Unfortunately...", font, windowSize, sf::Color::Red);
            break;

        default:
            break;
    }

    // Draw skill menu if active
    if (m_showingSkillMenu) {
        // Draw semi-transparent background
        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(windowSize.x, windowSize.y));
        overlay.setFillColor(sf::Color(0, 0, 0, 128));
        window.draw(overlay);

        // Draw skill menu title
        sf::Text menuTitle;
        menuTitle.setFont(font);
        menuTitle.setString(m_isDefenseSkillMenu ? "Choose Defense Skill:" : "Choose Attack Skill:");
        menuTitle.setCharacterSize(28);
        menuTitle.setFillColor(sf::Color::White);
        sf::FloatRect titleBounds = menuTitle.getLocalBounds();
        menuTitle.setPosition((windowSize.x - titleBounds.width) / 2.0f, windowSize.y / 2.0f - 50);
        window.draw(menuTitle);

        // Draw skill buttons
        if (m_isDefenseSkillMenu) {
            for (auto& button : m_defenseSkillButtons) {
                button.draw(window);
            }
        } else {
            for (auto& button : m_attackSkillButtons) {
                button.draw(window);
            }
        }
    }

    // Draw stat panels with fixed positioning
    std::string playerStatusText = (m_playerStatus.type == StatusEffect::Stun) ? "Stunned" :
                                  (m_playerStatus.type == StatusEffect::ATKBonus) ? "Buffed" : "Normal";
    std::string enemyStatusText = (m_enemyStatus.type == StatusEffect::Stun) ? "Stunned" : "Normal";

    CombatUI::StatData playerStats(m_playerName, m_playerHP, m_playerMaxHP, m_playerATK, m_playerDEF, playerStatusText);
    CombatUI::StatData enemyStats(m_enemyName, m_enemyHP, m_enemyMaxHP, m_enemyATK, m_enemyDEF, enemyStatusText);
    CombatUI::drawStatPanelTopLeft(window, playerStats, font);
    CombatUI::drawStatPanelBottomRight(window, enemyStats, font, windowSize);
}

bool CombatState::update(sf::Time dt) {
    // Update phase timers
    switch (m_phase) {
        case CombatPhase::ReadyBanner:
            m_bannerTimer += dt;
            if (m_bannerTimer >= m_bannerDuration) {
                m_phase = CombatPhase::PlayerCoinChoice;
                m_bannerTimer = sf::Time::Zero;
            }
            break;

        case CombatPhase::PlayerCoinFlip:
            m_coinTimer += dt;
            if (m_coinTimer >= m_coinDuration) {
                // Generate coin result
                m_coinResult = (getContext().rng->rollRange(1, 2) == 1) ? CoinResult::Head : CoinResult::Tail;
                m_coinCorrect = (m_playerChoice == CoinChoice::Head && m_coinResult == CoinResult::Head) ||
                               (m_playerChoice == CoinChoice::Tail && m_coinResult == CoinResult::Tail);

                // Determine if this is attack or defense coin
                if (m_isDefenseSkillMenu) {
                    // This is defense coin for enemy turn
                    resolveEnemyCoin(m_coinCorrect);
                } else {
                    // This is attack coin for player turn
                    resolvePlayerCoin(m_coinCorrect);
                }

                m_phase = CombatPhase::PlayerAction;
                m_coinTimer = sf::Time::Zero;
            }
            break;

        case CombatPhase::Victory:
        case CombatPhase::Defeat:
            m_resultTimer += dt;
            if (m_resultTimer >= m_resultDuration) {
                // Determine result based on current phase
                CombatResult result = CombatResult::None;
                if (m_phase == CombatPhase::Victory) {
                    result = CombatResult::Victory;
                } else if (m_phase == CombatPhase::Defeat) {
                    result = CombatResult::Unfortunately;
                }

                m_phase = CombatPhase::Ended;
                m_combatResult = result;

                // Set static result for MapState to read
                s_lastCombatResult = result;
                s_combatJustEnded = true;

                std::cout << "CombatState: Setting result to " << (result == CombatResult::Victory ? "Victory" : "Unfortunately") << std::endl;

                requestStackPop();
            }
            break;

        default:
            break;
    }

    // Update shake animations
    m_atkShakePika.update(dt);
    m_atkShakeEnemy.update(dt);
    m_hurtNudgePika.update(dt);
    m_hurtNudgeEnemy.update(dt);

    // Update status effects
    updateStatusEffects();

    // Remove old UI updates - only keep essential elements
    
    updateUI();
    
    return false;
}

bool CombatState::handleEvent(const sf::Event& event) {
    // Handle coin choice
    if (m_phase == CombatPhase::PlayerCoinChoice && event.type == sf::Event::MouseButtonPressed) {
        if (m_headButton.handleEvent(event)) {
            m_playerChoice = CoinChoice::Head;
            m_phase = CombatPhase::PlayerCoinFlip;
            m_coinTimer = sf::Time::Zero;
            return true;
        }
        if (m_tailButton.handleEvent(event)) {
            m_playerChoice = CoinChoice::Tail;
            m_phase = CombatPhase::PlayerCoinFlip;
            m_coinTimer = sf::Time::Zero;
            return true;
        }
    }

    // Handle skill menu selection
    if (m_showingSkillMenu && event.type == sf::Event::MouseButtonPressed) {
        if (m_isDefenseSkillMenu) {
            for (int i = 0; i < 3; ++i) {
                if (m_defenseSkillButtons[i].handleEvent(event)) {
                    applyPikaDefSkill(static_cast<PikaDefSkill>(i));
                    return true;
                }
            }
        } else {
            for (int i = 0; i < 3; ++i) {
                if (m_attackSkillButtons[i].handleEvent(event)) {
                    applyPikaAtkSkill(static_cast<PikaAtkSkill>(i));
                    return true;
                }
            }
        }
    }

    // Handle player action after coin flip
    if (m_phase == CombatPhase::PlayerAction && event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            performPlayerAttack();
            return true;
        }
    }

    // Temporary: Quick combat end for testing
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::V) {
            // V = Victory
            m_phase = CombatPhase::Victory;
            m_resultTimer = sf::Time::Zero;
            return true;
        }
        if (event.key.code == sf::Keyboard::U) {
            // U = Unfortunately
            m_phase = CombatPhase::Defeat;
            m_resultTimer = sf::Time::Zero;
            return true;
        }
    }

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
        m_enemyType = "boss";
    } else {
        m_enemy = Enemy::createRegularEnemy();
        m_enemyType = "chalamander"; // Default
    }

    // Reset to ready banner
    m_phase = CombatPhase::ReadyBanner;
    m_bannerTimer = sf::Time::Zero;

    addLogMessage("Combat begins!");
}

void CombatState::initializeCombat(Player& player, const std::string& enemyType) {
    m_player = &player;
    m_enemyType = enemyType;

    if (enemyType == "boss") {
        m_enemy = Boss::createBoss();
        m_isBoss = true;
    } else {
        m_enemy = Enemy::createRegularEnemy();
        m_isBoss = false;
    }

    // Reset to ready banner
    m_phase = CombatPhase::ReadyBanner;
    m_bannerTimer = sf::Time::Zero;

    addLogMessage("Combat begins!");
}

void CombatState::setupUI() {
    AssetManager& assets = *getContext().assets;

    // Setup coin choice buttons
    sf::Vector2f windowSize(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    float buttonWidth = 120;
    float buttonHeight = 50;
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f + 50;

    m_headButton.setPosition(sf::Vector2f(centerX - buttonWidth - 10, centerY));
    m_headButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_headButton.setText("HEAD");
    m_headButton.setFont(assets.getDefaultFont());

    m_tailButton.setPosition(sf::Vector2f(centerX + 10, centerY));
    m_tailButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    m_tailButton.setText("TAIL");
    m_tailButton.setFont(assets.getDefaultFont());

    // Setup attack skill buttons
    m_attackSkillButtons.resize(3);
    std::vector<std::string> attackSkillNames = {"Thunderbolt", "Electro Ball", "Volt Tackle"};
    for (int i = 0; i < 3; ++i) {
        m_attackSkillButtons[i].setPosition(sf::Vector2f(centerX - 180 + i * 120, centerY + 80));
        m_attackSkillButtons[i].setSize(sf::Vector2f(110, 40));
        m_attackSkillButtons[i].setText(attackSkillNames[i]);
        m_attackSkillButtons[i].setFont(assets.getDefaultFont());
    }

    // Setup defense skill buttons
    m_defenseSkillButtons.resize(3);
    std::vector<std::string> defenseSkillNames = {"Quick Guard", "Agility", "Charge"};
    for (int i = 0; i < 3; ++i) {
        m_defenseSkillButtons[i].setPosition(sf::Vector2f(centerX - 180 + i * 120, centerY + 80));
        m_defenseSkillButtons[i].setSize(sf::Vector2f(110, 40));
        m_defenseSkillButtons[i].setText(defenseSkillNames[i]);
        m_defenseSkillButtons[i].setFont(assets.getDefaultFont());
    }
    
    // Remove old panels - only keep compact stat panels
    
    // Remove old UI elements - only keep coin buttons and skill buttons
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
    m_phase = CombatPhase::PlayerCoinChoice;
    m_playerChoice = CoinChoice::None;
    m_coinResult = CoinResult::None;
}

void CombatState::startEnemyTurn() {
    m_phase = CombatPhase::EnemyAction;
    performEnemyAttack();
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

// Old methods removed - using performPlayerAttack and performEnemyAttack instead

void CombatState::checkCombatEnd() {
    if (m_enemyHP <= 0) {
        m_phase = CombatPhase::Victory;
        m_resultTimer = sf::Time::Zero;
        addLogMessage("Victory!");
    } else if (m_playerHP <= 0) {
        m_phase = CombatPhase::Defeat;
        m_resultTimer = sf::Time::Zero;
        addLogMessage("Unfortunately...");
    }
}

// Enhanced combat flow methods
void CombatState::beginPlayerTurn() {
    m_phase = CombatPhase::PlayerCoinChoice;
    m_playerChoice = CoinChoice::None;
    m_coinResult = CoinResult::None;
}

void CombatState::resolvePlayerCoin(bool correct) {
    m_coinCorrect = correct;
    if (correct) {
        // Show attack skill menu
        m_showingSkillMenu = true;
        m_isDefenseSkillMenu = false;
    } else {
        // Normal attack
        int damage = m_playerATK + (m_playerStatus.type == StatusEffect::ATKBonus ? m_playerStatus.magnitude : 0);
        m_enemyHP -= damage;
        if (m_enemyHP < 0) m_enemyHP = 0;

        triggerAttackShake(true);
        triggerHurtNudge(false);

        addLogMessage("Pikachu used Normal Attack for " + std::to_string(damage) + " damage!");

        // Clear ATK bonus after use
        if (m_playerStatus.type == StatusEffect::ATKBonus) {
            m_playerStatus = StatusEffect();
        }

        checkCombatEnd();
        if (m_phase != CombatPhase::Victory && m_phase != CombatPhase::Defeat) {
            beginEnemyTurn();
        }
    }
}

void CombatState::beginEnemyTurn() {
    // Check if enemy is stunned
    if (m_enemyStatus.type == StatusEffect::Stun && m_enemyStatus.duration > 0) {
        addLogMessage("Enemy is stunned and loses turn!");
        m_enemyStatus.duration--;
        if (m_enemyStatus.duration <= 0) {
            m_enemyStatus = StatusEffect();
        }
        beginPlayerTurn();
        return;
    }

    // Enemy turn: Player gets to choose defense coin
    addLogMessage("Enemy's turn! Choose HEAD/TAIL for defense!");
    m_phase = CombatPhase::PlayerCoinChoice; // Player chooses defense coin
    m_playerChoice = CoinChoice::None;
    m_coinResult = CoinResult::None;
    m_isDefenseSkillMenu = true; // Flag that this is for defense
}

void CombatState::resolveEnemyCoin(bool correct) {
    if (correct) {
        // Show defense skill menu for Pikachu
        m_showingSkillMenu = true;
        m_isDefenseSkillMenu = true;
        addLogMessage("Coin correct! Choose defense skill!");
    } else {
        // Take full damage from enemy attack
        addLogMessage("Coin wrong! Taking full damage!");

        // Enemy 30% dodge chance when defending
        if (enemyTryDodge()) {
            addLogMessage("Enemy dodged the attack!");
            // No damage, just shake enemy slightly
            triggerAttackShake(false);
        } else {
            // Take full enemy attack
            performEnemyAttack();
        }
    }
}

bool CombatState::enemyTryDodge() {
    return getContext().rng->rollRange(1, 100) <= 30;
}

void CombatState::updateStatusEffects() {
    // Update player status
    if (m_playerStatus.type != StatusEffect::None && m_playerStatus.duration > 0) {
        // Status effects are updated per turn, not per frame
    }

    // Update enemy status
    if (m_enemyStatus.type != StatusEffect::None && m_enemyStatus.duration > 0) {
        // Status effects are updated per turn, not per frame
    }
}

void CombatState::triggerAttackShake(bool isPlayer) {
    if (isPlayer) {
        m_atkShakePika.start(sf::Vector2f(80.0f, getContext().window->getSize().y - 40.0f));
    } else {
        m_atkShakeEnemy.start(sf::Vector2f(getContext().window->getSize().x - 80.0f, 80.0f));
    }
}

void CombatState::triggerHurtNudge(bool isPlayer) {
    if (isPlayer) {
        m_hurtNudgePika.start(sf::Vector2f(-1, 0)); // Nudge left
    } else {
        m_hurtNudgeEnemy.start(sf::Vector2f(1, 0)); // Nudge right
    }
}

// Skill implementations
void CombatState::applyPikaAtkSkill(PikaAtkSkill skill) {
    int damage = 0;
    std::string skillName;

    switch (skill) {
        case PikaAtkSkill::Thunderbolt:
            damage = static_cast<int>(m_playerATK * 2.0f);
            skillName = "Thunderbolt";
            // 20% chance to stun
            if (getContext().rng->rollRange(1, 100) <= 20) {
                m_enemyStatus = StatusEffect(StatusEffect::Stun, 1);
                addLogMessage("Enemy is stunned!");
            }
            break;

        case PikaAtkSkill::ElectroBall:
            {
                int hpLost = m_enemyMaxHP - m_enemyHP;
                damage = m_playerATK + static_cast<int>(0.15f * hpLost);
                skillName = "Electro Ball";
            }
            break;

        case PikaAtkSkill::VoltTackle:
            damage = static_cast<int>(m_playerATK * 2.5f);
            skillName = "Volt Tackle";
            // 10% recoil damage
            int recoil = static_cast<int>(damage * 0.1f);
            m_playerHP -= recoil;
            if (m_playerHP < 0) m_playerHP = 0;
            addLogMessage("Pikachu takes " + std::to_string(recoil) + " recoil damage!");
            break;
    }

    // Apply ATK bonus if active
    if (m_playerStatus.type == StatusEffect::ATKBonus) {
        damage += m_playerStatus.magnitude;
        m_playerStatus = StatusEffect(); // Clear after use
    }

    m_enemyHP -= damage;
    if (m_enemyHP < 0) m_enemyHP = 0;

    triggerAttackShake(true);
    triggerHurtNudge(false);

    addLogMessage("Pikachu used " + skillName + " for " + std::to_string(damage) + " damage!");

    m_showingSkillMenu = false;
    checkCombatEnd();
    if (m_phase != CombatPhase::Victory && m_phase != CombatPhase::Defeat) {
        beginEnemyTurn();
    }
}

void CombatState::applyPikaDefSkill(PikaDefSkill skill) {
    std::string skillName;
    int enemyDamage = m_enemyATK;
    int finalDamage = enemyDamage;

    switch (skill) {
        case PikaDefSkill::QuickGuard:
            finalDamage = static_cast<int>(enemyDamage * 0.5f);
            skillName = "Quick Guard";
            addLogMessage("Pikachu used Quick Guard! Damage reduced by 50%!");
            break;

        case PikaDefSkill::Agility:
            finalDamage = 0;
            skillName = "Agility";
            addLogMessage("Pikachu used Agility! Attack dodged completely!");
            break;

        case PikaDefSkill::Charge:
            finalDamage = static_cast<int>(enemyDamage * 0.7f);
            m_playerStatus = StatusEffect(StatusEffect::ATKBonus, 1, 10);
            skillName = "Charge";
            addLogMessage("Pikachu used Charge! Damage reduced and ATK boosted!");
            break;
    }

    m_playerHP -= finalDamage;
    if (m_playerHP < 0) m_playerHP = 0;

    if (finalDamage > 0) {
        triggerAttackShake(false);
        triggerHurtNudge(true);
        addLogMessage("Pikachu takes " + std::to_string(finalDamage) + " damage!");
    }

    m_showingSkillMenu = false;
    checkCombatEnd();
    if (m_phase != CombatPhase::Victory && m_phase != CombatPhase::Defeat) {
        beginPlayerTurn();
    }
}

void CombatState::endCombat(CombatResult result) {
    m_phase = CombatPhase::Ended;
    
    switch (result) {
        case CombatResult::Victory:
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
        case CombatResult::Unfortunately:
            addLogMessage("You were defeated!");
            requestStateClear();
            requestStackPush(StateID::GameOver);
            break;
        default:
            break;
    }
}

void CombatState::onNormalAttackSelected() {
    performPlayerAttack();
}

void CombatState::onSkillSelected(SkillType skillType) {
    useSkill(skillType);
}

void CombatState::drawCombatSprites(sf::RenderWindow& window, const AssetManager& assets) {
    sf::Vector2u winSize = window.getSize();

    // Fixed sprite heights (no scaling based on window size)
    const float PIKACHU_HEIGHT = 300.0f;  // ±20 as per spec
    const float ENEMY_HEIGHT = 320.0f;    // ±20 as per spec
    const float MARGIN_X = 80.0f;
    const float MARGIN_Y_BOTTOM = 40.0f;
    const float MARGIN_Y_TOP = 100.0f;

    // Base positions with fixed margins (as per spec)
    sf::Vector2f playerBasePos(MARGIN_X, winSize.y - MARGIN_Y_BOTTOM);  // Bottom-left
    sf::Vector2f enemyBasePos(winSize.x - MARGIN_X, MARGIN_Y_TOP);      // Top-right

    // Create player sprite (Pikachu) - fixed height, bottom-left anchor
    sf::Sprite playerSprite = assets.makeSprite("player_combat");

    // Fixed scaling to maintain form
    sf::Vector2u playerTexSize = playerSprite.getTexture()->getSize();
    float playerScale = PIKACHU_HEIGHT / playerTexSize.y;
    playerSprite.setScale(playerScale, playerScale);

    // Apply shake and nudge offsets
    sf::Vector2f playerOffset = m_atkShakePika.getOffset() + m_hurtNudgePika.getOffset();
    sf::Vector2f finalPlayerPos = playerBasePos + playerOffset;

    // Manual bottom-left positioning
    sf::FloatRect playerBounds = playerSprite.getLocalBounds();
    playerSprite.setPosition(
        finalPlayerPos.x - playerBounds.left,
        finalPlayerPos.y - (playerBounds.top + playerBounds.height)
    );

    // Enable alpha blending
    sf::RenderStates playerStates;
    playerStates.blendMode = sf::BlendAlpha;
    window.draw(playerSprite, playerStates);

    // Create enemy sprite - fixed height, top-right anchor
    std::string enemyKey = "monster_" + m_enemyType;
    sf::Sprite enemySprite = assets.makeSprite(enemyKey);

    // Fixed scaling to maintain form
    sf::Vector2u enemyTexSize = enemySprite.getTexture()->getSize();
    float enemyScale = ENEMY_HEIGHT / enemyTexSize.y;
    enemySprite.setScale(enemyScale, enemyScale);

    // Apply shake and nudge offsets
    sf::Vector2f enemyOffset = m_atkShakeEnemy.getOffset() + m_hurtNudgeEnemy.getOffset();
    sf::Vector2f finalEnemyPos = enemyBasePos + enemyOffset;

    // Manual top-right positioning
    sf::FloatRect enemyBounds = enemySprite.getLocalBounds();
    enemySprite.setPosition(
        finalEnemyPos.x - (enemyBounds.left + enemyBounds.width),
        finalEnemyPos.y - enemyBounds.top
    );

    // Enable alpha blending for enemy sprite
    sf::RenderStates enemyStates;
    enemyStates.blendMode = sf::BlendAlpha;
    window.draw(enemySprite, enemyStates);
}

void CombatState::performPlayerAttack() {
    int damage = m_coinCorrect ? (m_playerATK * 2) : m_playerATK; // Special vs Normal
    m_enemyHP -= damage;
    if (m_enemyHP < 0) m_enemyHP = 0;

    addLogMessage(m_coinCorrect ? "Pikachu used Special Attack!" : "Pikachu used Normal Attack!");
    addLogMessage("Enemy takes " + std::to_string(damage) + " damage!");

    checkCombatEnd();
    if (m_phase != CombatPhase::Victory && m_phase != CombatPhase::Defeat) {
        m_phase = CombatPhase::EnemyAction;
        performEnemyAttack();
    }
}

void CombatState::performEnemyAttack() {
    // Enemy only has normal attack (as per spec)
    int damage = m_enemyATK;

    // Check if player has defense skill active or enemy dodges
    bool playerDodged = false;

    // Apply any active defense effects would be handled in resolveEnemyCoin
    // For now, just do normal attack

    triggerAttackShake(false);  // Enemy attacks
    triggerHurtNudge(true);     // Player gets hurt

    m_playerHP -= damage;
    if (m_playerHP < 0) m_playerHP = 0;

    addLogMessage("Enemy attacks for " + std::to_string(damage) + " damage!");

    checkCombatEnd();
    if (m_phase != CombatPhase::Victory && m_phase != CombatPhase::Defeat) {
        // Reset for next player turn
        m_playerChoice = CoinChoice::None;
        m_coinResult = CoinResult::None;
        m_phase = CombatPhase::PlayerCoinChoice;
    }
}


