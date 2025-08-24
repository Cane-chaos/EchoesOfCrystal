#pragma once
#include "State.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include "ui/Bar.h"
#include "ui/Panel.h"
#include "ui/CombatUI.h"
#include <vector>
#include <memory>
#include <cmath>

enum class PikaAtkSkill {
    Thunderbolt,
    ElectroBall,
    VoltTackle
};

enum class PikaDefSkill {
    QuickGuard,
    Agility,
    Charge
};

struct StatusEffect {
    enum Type { None, Stun, ATKBonus } type;
    int duration;
    int magnitude;

    StatusEffect() : type(None), duration(0), magnitude(0) {}
    StatusEffect(Type t, int dur, int mag = 0) : type(t), duration(dur), magnitude(mag) {}
};

class CombatState : public State {
public:
    CombatState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    // Setup
    void initializeCombat(Player& player, bool isBoss = false);
    void initializeCombat(Player& player, const std::string& enemyType);

    // Result
    CombatResult getCombatResult() const { return m_combatResult; }

    // Static result for communication with MapState
    static CombatResult s_lastCombatResult;
    static bool s_combatJustEnded;
    
private:
    void setupUI();
    void updateUI();
    void addLogMessage(const std::string& message);
    void drawCombatSprites(sf::RenderWindow& window, const class AssetManager& assets);
    void performPlayerAttack();
    void performEnemyAttack();
    void checkCombatEnd();

    // Enhanced combat flow
    void beginPlayerTurn();
    void resolvePlayerCoin(bool correct);
    void beginEnemyTurn();
    void resolveEnemyCoin(bool correct);

    // Skill system
    void applyPikaAtkSkill(PikaAtkSkill skill);
    void applyPikaDefSkill(PikaDefSkill skill);
    bool enemyTryDodge();
    void updateStatusEffects();

    // Animation helpers
    void triggerAttackShake(bool isPlayer);
    void triggerHurtNudge(bool isPlayer);
    
    // Combat flow
    void startPlayerTurn();
    void startEnemyTurn();
    void performCoinFlip();
    void onCoinResult(bool playerWon);
    void showSkillSelection();
    void hideSkillSelection();
    void useSkill(SkillType skillType);

    void endCombat(CombatResult result);
    
    // UI callbacks
    void onNormalAttackSelected();
    void onSkillSelected(SkillType skillType);
    
private:
    Player* m_player;
    std::unique_ptr<class Enemy> m_enemy;
    bool m_isBoss;
    
    enum class CombatPhase {
        ReadyBanner,
        PlayerCoinChoice,
        PlayerCoinFlip,
        PlayerAction,
        EnemyAction,
        Victory,
        Defeat,
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

    // Enhanced combat visuals
    std::string m_enemyType;  // "bisasam", "chalamander", "boss"
    sf::Time m_bannerTimer;
    sf::Time m_bannerDuration;

    // Coin flip mechanics
    enum class CoinChoice { None, Head, Tail } m_playerChoice;
    enum class CoinResult { None, Head, Tail } m_coinResult;
    sf::Time m_coinTimer;
    sf::Time m_coinDuration;
    bool m_coinCorrect;

    // Combat stats
    int m_playerHP, m_playerMaxHP, m_playerATK, m_playerDEF;
    int m_enemyHP, m_enemyMaxHP, m_enemyATK, m_enemyDEF;
    std::string m_playerName, m_enemyName;

    // UI buttons
    UI::Button m_headButton;
    UI::Button m_tailButton;

    // Result display
    sf::Time m_resultTimer;
    sf::Time m_resultDuration;
    CombatResult m_combatResult;

    // Shake animation system
    struct Shake {
        bool active = false;
        float t = 0.0f;
        float duration = 0.18f;
        float amplitude = 10.0f;
        sf::Vector2f basePosition;

        sf::Vector2f getOffset() const {
            if (!active) return sf::Vector2f(0, 0);
            float progress = t / duration;
            float shakeX = std::sin(progress * 3.14159f * 4) * amplitude * (1.0f - progress);
            return sf::Vector2f(shakeX, 0);
        }

        void start(sf::Vector2f base) {
            active = true;
            t = 0.0f;
            basePosition = base;
        }

        void update(sf::Time dt) {
            if (active) {
                t += dt.asSeconds();
                if (t >= duration) {
                    active = false;
                    t = 0.0f;
                }
            }
        }
    };

    struct Nudge {
        bool active = false;
        float t = 0.0f;
        float duration = 0.12f;
        float distance = 6.0f;
        sf::Vector2f direction;

        sf::Vector2f getOffset() const {
            if (!active) return sf::Vector2f(0, 0);
            float progress = t / duration;
            float factor = 1.0f - progress; // Linear fade out
            return direction * distance * factor;
        }

        void start(sf::Vector2f dir) {
            active = true;
            t = 0.0f;
            direction = dir;
        }

        void update(sf::Time dt) {
            if (active) {
                t += dt.asSeconds();
                if (t >= duration) {
                    active = false;
                    t = 0.0f;
                }
            }
        }
    };

    Shake m_atkShakePika, m_atkShakeEnemy;
    Nudge m_hurtNudgePika, m_hurtNudgeEnemy;

    // Status effects
    StatusEffect m_playerStatus, m_enemyStatus;

    // Skill selection
    bool m_showingSkillMenu;
    bool m_isDefenseSkillMenu;
    std::vector<UI::Button> m_attackSkillButtons;
    std::vector<UI::Button> m_defenseSkillButtons;
};
