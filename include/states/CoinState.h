#pragma once
#include "State.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"
#include <functional>

class CoinState : public State {
public:
    using ResultCallback = std::function<void(bool)>; // true if player guessed correctly
    
public:
    CoinState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    void setResultCallback(ResultCallback callback);
    
private:
    void setupUI();
    void onHeadSelected();
    void onTailSelected();
    void startFlipAnimation();
    void finishAnimation();
    
private:
    UI::TextLabel m_instructionLabel;
    UI::TextLabel m_coinLabel;
    UI::TextLabel m_resultLabel;
    UI::Button m_headButton;
    UI::Button m_tailButton;
    
    enum class Phase {
        Selection,
        Flipping,
        Result
    } m_phase;
    
    sf::Time m_animationTime;
    sf::Time m_totalAnimationTime;
    
    CoinChoice m_playerChoice;
    bool m_coinResult; // true = heads, false = tails
    bool m_playerWon;
    
    ResultCallback m_resultCallback;
};
