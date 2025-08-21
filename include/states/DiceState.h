#pragma once
#include "State.h"
#include "ui/TextLabel.h"
#include <functional>

class DiceState : public State {
public:
    using ResultCallback = std::function<void(int)>;
    
public:
    DiceState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    void setResultCallback(ResultCallback callback);
    
private:
    void startAnimation();
    void finishAnimation();
    
private:
    UI::TextLabel m_diceLabel;
    UI::TextLabel m_instructionLabel;
    
    sf::Time m_animationTime;
    sf::Time m_totalAnimationTime;
    bool m_animating;
    int m_finalResult;
    
    ResultCallback m_resultCallback;
};
