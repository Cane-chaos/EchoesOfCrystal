#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace CombatUI {
    // Stat panel data
    struct StatData {
        std::string name;
        int currentHP;
        int maxHP;
        int attack;
        int defense;
        std::string status;

        StatData() : name("Unknown"), currentHP(100), maxHP(100), attack(10), defense(8), status("Normal") {}
        StatData(const std::string& n, int hp, int maxHp, int atk, int def, const std::string& st = "Normal")
            : name(n), currentHP(hp), maxHP(maxHp), attack(atk), defense(def), status(st) {}
    };
    
    // Panel drawing functions
    void drawStatPanelTopLeft(sf::RenderTarget& target, const StatData& stats, const sf::Font& font, 
                             sf::Vector2f position = sf::Vector2f(40, 40), 
                             sf::Vector2f size = sf::Vector2f(360, 120));
                             
    void drawStatPanelBottomRight(sf::RenderTarget& target, const StatData& stats, const sf::Font& font,
                                 sf::Vector2f windowSize,
                                 sf::Vector2f size = sf::Vector2f(360, 120));
    
    // Banner drawing
    void drawBanner(sf::RenderTarget& target, const std::string& text, const sf::Font& font,
                   sf::Vector2f windowSize, sf::Color textColor = sf::Color::White);

    // HP Bar drawing
    void drawHPBar(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size,
                   int currentHP, int maxHP, sf::Color barColor = sf::Color::Red);

    // Coin flip UI
    void drawCoinChoice(sf::RenderTarget& target, const sf::Font& font, sf::Vector2f windowSize);
    void drawCoinFlipping(sf::RenderTarget& target, const sf::Font& font, sf::Vector2f windowSize);
    void drawCoinResult(sf::RenderTarget& target, const sf::Font& font, sf::Vector2f windowSize,
                       bool isHead, bool correct);

    // Helper functions
    sf::RectangleShape createPanel(sf::Vector2f position, sf::Vector2f size,
                                  sf::Color fillColor = sf::Color(0, 0, 0, 160),
                                  sf::Color outlineColor = sf::Color::White,
                                  float outlineThickness = 2.0f);
}
