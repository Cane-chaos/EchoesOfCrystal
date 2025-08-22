#pragma once
#include "State.h"
#include "ui/TextLabel.h"
#include <functional>

class ReadyState : public State {
public:
    using ReadyCallback = std::function<void()>;
    
public:
    ReadyState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
    void setReadyCallback(ReadyCallback callback);
    void setDuration(sf::Time duration);
    
private:
    void setupUI();
    void finishReady();
    
private:
    UI::TextLabel m_readyLabel;
    UI::TextLabel m_countdownLabel;
    
    sf::Time m_timer;
    sf::Time m_duration;
    bool m_finished;
    
    ReadyCallback m_readyCallback;
};
