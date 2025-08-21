#pragma once
#include "State.h"
#include "ui/Button.h"
#include "ui/TextLabel.h"
#include <vector>

class MenuState : public State {
public:
    MenuState(StateStack& stack, Context context);
    
    virtual void draw() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    
private:
    void setupUI();
    void onStartGame();
    void onLoadGame();
    void onExit();
    
private:
    sf::Sprite m_backgroundSprite;
    UI::TextLabel m_titleLabel;
    
    std::vector<UI::Button> m_buttons;
    size_t m_selectedButton;

    bool m_showPokemonSelection;
    std::vector<UI::Button> m_pokemonButtons;
    size_t m_selectedPokemon;
};
