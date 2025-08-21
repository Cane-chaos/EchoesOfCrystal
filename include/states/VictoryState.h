#pragma once
#include "State.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"

class VictoryState : public State {
public:
    VictoryState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
private:
    void setupUI();
    void onExit();
    
private:
    UI::TextLabel m_titleLabel;
    UI::TextLabel m_messageLabel;
    UI::Button m_exitButton;
};
