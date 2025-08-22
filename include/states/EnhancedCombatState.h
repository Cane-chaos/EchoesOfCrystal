#pragma once
#include "State.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include "ui/Panel.h"
#include <vector>
#include <memory>

class Player;
class Enemy;

class EnhancedCombatState : public State {
public:
    EnhancedCombatState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    // Setup
    void initializeCombat(Player& player, MonsterType enemyType);
    
private:
    void setupUI();
    void updateUI();
    void addLogMessage(const std::string& message);
    
    // Combat phases
    void startReadyPhase();
    void startPlayerCoinPhase();
    void startPlayerActionPhase(bool coinSuccess);
    void startEnemyCoinPhase();
    void startEnemyActionPhase(bool coinSuccess);
    void showVictoryPhase();
    void showDefeatPhase();
    void endCombat();
    
    // Coin flip mechanics
    void performCoinFlip(bool isPlayerTurn);
    void onCoinResult(bool playerGuessedCorrect, bool isPlayerTurn);
    
    // Action selection
    void showSkillSelection(bool isAttack);
    void hideSkillSelection();
    void useSkill(SkillType skillType);
    void performNormalAttack();
    void performDefense(SkillType defenseSkill);
    
    // Enemy AI
    void performEnemyAction();
    SkillType selectEnemySkill() const;
    
    // UI callbacks
    void onHeadSelected();
    void onTailSelected();
    void onSkillSelected(SkillType skillType);
    void onNormalAttackSelected();
    
private:
    Player* m_player;
    std::unique_ptr<Enemy> m_enemy;
    MonsterType m_enemyType;
    
    CombatPhase m_phase;
    sf::Time m_phaseTimer;
    sf::Time m_readyDuration;
    sf::Time m_coinDuration;
    
    bool m_waitingForInput;
    bool m_coinFlipInProgress;
    bool m_playerCoinGuess; // true = heads, false = tails
    bool m_currentCoinResult;
    bool m_isPlayerTurn;
    
    // UI elements
    UI::Panel m_backgroundPanel;
    UI::Panel m_playerPanel;
    UI::Panel m_enemyPanel;
    UI::Panel m_logPanel;
    UI::Panel m_skillPanel;
    UI::Panel m_coinPanel;
    
    // Character display
    UI::TextLabel m_playerNameLabel;
    UI::TextLabel m_enemyNameLabel;
    UI::Bar m_playerHPBar;
    UI::Bar m_playerMPBar;
    UI::Bar m_enemyHPBar;
    UI::Bar m_enemyMPBar;
    
    // Combat messages
    UI::TextLabel m_readyLabel;
    UI::TextLabel m_phaseLabel;
    UI::TextLabel m_coinLabel;
    UI::TextLabel m_resultLabel;
    std::vector<UI::TextLabel> m_logMessages;
    
    // Action buttons
    UI::Button m_headButton;
    UI::Button m_tailButton;
    UI::Button m_normalAttackButton;
    std::vector<UI::Button> m_skillButtons;
    
    bool m_skillSelectionVisible;
    bool m_coinSelectionVisible;
};
