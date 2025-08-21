#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace UI {

class Bar {
public:
    Bar();
    Bar(const sf::Vector2f& position, const sf::Vector2f& size);
    
    // Setup
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    void setValues(float current, float maximum);
    void setCurrentValue(float current);
    void setMaximumValue(float maximum);
    
    // Appearance
    void setBackgroundColor(const sf::Color& color);
    void setFillColor(const sf::Color& color);
    void setBorderColor(const sf::Color& color);
    void setBorderThickness(float thickness);
    
    // Text
    void setFont(const sf::Font& font);
    void setTextColor(const sf::Color& color);
    void setCharacterSize(unsigned int size);
    void setShowText(bool show);
    void setTextFormat(const std::string& format); // e.g., "{current}/{max}" or "{percent}%"
    
    // Animation
    void setAnimated(bool animated);
    void setAnimationSpeed(float speed);
    void update(sf::Time dt);
    
    // Rendering
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    
    // Getters
    float getCurrentValue() const { return m_currentValue; }
    float getMaximumValue() const { return m_maximumValue; }
    float getPercentage() const;
    sf::FloatRect getBounds() const;
    
private:
    void updateText();
    void updateFillWidth();
    
private:
    sf::RectangleShape m_background;
    sf::RectangleShape m_fill;
    sf::Text m_text;
    
    float m_currentValue;
    float m_maximumValue;
    float m_displayValue; // For animation
    
    std::string m_textFormat;
    bool m_showText;
    bool m_animated;
    float m_animationSpeed;
};

} // namespace UI
