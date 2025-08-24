#include "ui/CombatUI.h"
#include <sstream>

namespace CombatUI {

void drawStatPanelTopLeft(sf::RenderTarget& target, const StatData& stats, const sf::Font& font,
                         sf::Vector2f position, sf::Vector2f size) {
    // Use fixed position with 20px padding from top-left
    sf::Vector2f panelPos(20, 20);
    sf::Vector2f panelSize(280, 100); // Compact size

    // Create panel background
    sf::RectangleShape panel = createPanel(panelPos, panelSize);
    target.draw(panel);

    // Text settings
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);

    float textY = panelPos.y + 8;
    float lineHeight = 20;

    // Name
    text.setString(stats.name);
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);
    textY += lineHeight;

    // HP with bar
    std::ostringstream hpStream;
    hpStream << "HP: " << stats.currentHP << "/" << stats.maxHP;
    text.setString(hpStream.str());
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);

    // HP Bar
    drawHPBar(target, sf::Vector2f(panelPos.x + 8, textY + 18), sf::Vector2f(panelSize.x - 16, 6),
              stats.currentHP, stats.maxHP);
    textY += lineHeight + 8;

    // Attack & Defense
    std::ostringstream atkDefStream;
    atkDefStream << "ATK: " << stats.attack << " DEF: " << stats.defense;
    text.setString(atkDefStream.str());
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);
    textY += lineHeight;

    // Status
    text.setString("Status: " + stats.status);
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);
}

void drawStatPanelBottomRight(sf::RenderTarget& target, const StatData& stats, const sf::Font& font,
                             sf::Vector2f windowSize, sf::Vector2f size) {
    // Use fixed position with 20px padding from bottom-right
    sf::Vector2f panelSize(280, 100); // Compact size
    sf::Vector2f panelPos(windowSize.x - panelSize.x - 20, windowSize.y - panelSize.y - 20);

    // Create panel background
    sf::RectangleShape panel = createPanel(panelPos, panelSize);
    target.draw(panel);

    // Text settings
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);

    float textY = panelPos.y + 8;
    float lineHeight = 20;

    // Name
    text.setString(stats.name);
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);
    textY += lineHeight;

    // HP with bar
    std::ostringstream hpStream;
    hpStream << "HP: " << stats.currentHP << "/" << stats.maxHP;
    text.setString(hpStream.str());
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);

    // HP Bar
    drawHPBar(target, sf::Vector2f(panelPos.x + 8, textY + 18), sf::Vector2f(panelSize.x - 16, 6),
              stats.currentHP, stats.maxHP);
    textY += lineHeight + 8;

    // Attack & Defense
    std::ostringstream atkDefStream;
    atkDefStream << "ATK: " << stats.attack << " DEF: " << stats.defense;
    text.setString(atkDefStream.str());
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);
    textY += lineHeight;

    // Status
    text.setString("Status: " + stats.status);
    text.setPosition(panelPos.x + 8, textY);
    target.draw(text);
}

void drawBanner(sf::RenderTarget& target, const std::string& text, const sf::Font& font,
               sf::Vector2f windowSize, sf::Color textColor) {
    sf::Text bannerText;
    bannerText.setFont(font);
    bannerText.setString(text);
    bannerText.setCharacterSize(48);
    bannerText.setFillColor(textColor);
    bannerText.setStyle(sf::Text::Bold);
    
    // Add outline for better visibility
    bannerText.setOutlineThickness(2.0f);
    bannerText.setOutlineColor(sf::Color::Black);
    
    // Center the text
    sf::FloatRect textBounds = bannerText.getLocalBounds();
    bannerText.setPosition(
        (windowSize.x - textBounds.width) / 2.0f - textBounds.left,
        (windowSize.y - textBounds.height) / 2.0f - textBounds.top
    );
    
    target.draw(bannerText);
}

void drawHPBar(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size,
               int currentHP, int maxHP, sf::Color barColor) {
    // Background (black)
    sf::RectangleShape background;
    background.setPosition(position);
    background.setSize(size);
    background.setFillColor(sf::Color::Black);
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(1.0f);
    target.draw(background);

    // Foreground (HP bar)
    if (maxHP > 0) {
        float hpRatio = static_cast<float>(currentHP) / maxHP;
        sf::RectangleShape foreground;
        foreground.setPosition(position);
        foreground.setSize(sf::Vector2f(size.x * hpRatio, size.y));
        foreground.setFillColor(barColor);
        target.draw(foreground);
    }
}

void drawCoinChoice(sf::RenderTarget& target, const sf::Font& font, sf::Vector2f windowSize) {
    sf::Text text;
    text.setFont(font);
    text.setString("Choose: HEAD or TAIL");
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        (windowSize.x - textBounds.width) / 2.0f - textBounds.left,
        (windowSize.y - textBounds.height) / 2.0f - textBounds.top - 50
    );

    target.draw(text);
}

void drawCoinFlipping(sf::RenderTarget& target, const sf::Font& font, sf::Vector2f windowSize) {
    sf::Text text;
    text.setFont(font);
    text.setString("Flipping...");
    text.setCharacterSize(36);
    text.setFillColor(sf::Color::Yellow);
    text.setStyle(sf::Text::Bold);

    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        (windowSize.x - textBounds.width) / 2.0f - textBounds.left,
        (windowSize.y - textBounds.height) / 2.0f - textBounds.top
    );

    target.draw(text);
}

void drawCoinResult(sf::RenderTarget& target, const sf::Font& font, sf::Vector2f windowSize,
                   bool isHead, bool correct) {
    std::string resultText = "Result: " + std::string(isHead ? "HEAD" : "TAIL");
    std::string statusText = correct ? "CORRECT! Special Attack!" : "WRONG! Normal Attack!";

    sf::Text result;
    result.setFont(font);
    result.setString(resultText);
    result.setCharacterSize(32);
    result.setFillColor(sf::Color::White);
    result.setStyle(sf::Text::Bold);

    sf::FloatRect resultBounds = result.getLocalBounds();
    result.setPosition(
        (windowSize.x - resultBounds.width) / 2.0f - resultBounds.left,
        (windowSize.y - resultBounds.height) / 2.0f - resultBounds.top - 30
    );

    sf::Text status;
    status.setFont(font);
    status.setString(statusText);
    status.setCharacterSize(24);
    status.setFillColor(correct ? sf::Color::Green : sf::Color::Red);
    status.setStyle(sf::Text::Bold);

    sf::FloatRect statusBounds = status.getLocalBounds();
    status.setPosition(
        (windowSize.x - statusBounds.width) / 2.0f - statusBounds.left,
        (windowSize.y - statusBounds.height) / 2.0f - statusBounds.top + 30
    );

    target.draw(result);
    target.draw(status);
}

sf::RectangleShape createPanel(sf::Vector2f position, sf::Vector2f size,
                              sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
    sf::RectangleShape panel;
    panel.setPosition(position);
    panel.setSize(size);
    panel.setFillColor(fillColor);
    panel.setOutlineColor(outlineColor);
    panel.setOutlineThickness(outlineThickness);
    return panel;
}

} // namespace CombatUI
