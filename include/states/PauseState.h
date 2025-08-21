#pragma once
#include "State.h"
#include "ui/TextLabel.h"
#include "ui/Button.h"

class PauseState : public State {
public:
    PauseState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
private:
    void setupUI();
    void updateButtonSelection();
    void onResume();
    void onSave();
    void onMainMenu();
    
private:
    UI::TextLabel m_titleLabel;
    UI::Button m_resumeButton;
    UI::Button m_saveButton;
    UI::Button m_mainMenuButton;
    
    int m_selectedButton;
};
