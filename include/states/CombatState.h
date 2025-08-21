#pragma once
#include "State.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include "ui/Panel.h"
#include <vector>
#include <memory>

class CombatState : public State {
public:
    CombatState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    // Setup
    void initializeCombat(Player& player, bool isBoss = false);
    
private:
    void setupUI();
    void updateUI();
    void addLogMessage(const std::string& message);
    
    // Combat flow
    void startPlayerTurn();
    void startEnemyTurn();
    void performCoinFlip();
    void onCoinResult(bool playerWon);
    void showSkillSelection();
    void hideSkillSelection();
    void useSkill(SkillType skillType);
    void performNormalAttack();
    void performEnemyAction();
    void checkCombatEnd();
    void endCombat(CombatResult result);
    
    // UI callbacks
    void onNormalAttackSelected();
    void onSkillSelected(SkillType skillType);
    
private:
    Player* m_player;
    std::unique_ptr<class Enemy> m_enemy;
    bool m_isBoss;
    
    enum class CombatPhase {
        PlayerCoin,
        PlayerAction,
        EnemyAction,
        Ended
    } m_phase;
    
    // UI elements
    UI::Panel m_playerPanel;
    UI::Panel m_enemyPanel;
    UI::Panel m_logPanel;
    UI::Panel m_skillPanel;
    
    UI::TextLabel m_playerNameLabel;
    UI::TextLabel m_enemyNameLabel;
    UI::Bar m_playerHPBar;
    UI::Bar m_playerMPBar;
    UI::Bar m_enemyHPBar;
    UI::Bar m_enemyMPBar;
    
    std::vector<UI::TextLabel> m_logMessages;
    UI::Button m_normalAttackButton;
    std::vector<UI::Button> m_skillButtons;
    
    bool m_skillSelectionVisible;
    bool m_waitingForInput;
};
