#pragma once
#include <SFML/Graphics.hpp>

namespace UI {

class Panel {
public:
    Panel();
    Panel(const sf::Vector2f& position, const sf::Vector2f& size);
    
    // Setup
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    
    // Appearance
    void setFillColor(const sf::Color& color);
    void setOutlineColor(const sf::Color& color);
    void setOutlineThickness(float thickness);
    
    // Rendering
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    
    // Utility
    sf::FloatRect getBounds() const;
    bool contains(const sf::Vector2f& point) const;
    
private:
    sf::RectangleShape m_shape;
};

} // namespace UI
