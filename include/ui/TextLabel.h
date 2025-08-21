#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace UI {

class TextLabel {
public:
    TextLabel();
    TextLabel(const sf::Vector2f& position, const std::string& text);
    
    // Setup
    void setPosition(const sf::Vector2f& position);
    void setText(const std::string& text);
    void setFont(const sf::Font& font);
    
    // Appearance
    void setCharacterSize(unsigned int size);
    void setFillColor(const sf::Color& color);
    void setOutlineColor(const sf::Color& color);
    void setOutlineThickness(float thickness);
    void setStyle(sf::Uint32 style);
    
    // Alignment
    enum class Alignment {
        Left,
        Center,
        Right
    };
    
    void setAlignment(Alignment alignment);
    void setOrigin(const sf::Vector2f& origin);
    
    // Rendering
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    
    // Utility
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    std::string getString() const;  // Return by value to avoid temporary reference
    const sf::String& getSfString() const;  // Return SFML string directly
    
private:
    void updateAlignment();
    
private:
    sf::Text m_text;
    Alignment m_alignment;
    sf::Vector2f m_basePosition;
};

} // namespace UI
