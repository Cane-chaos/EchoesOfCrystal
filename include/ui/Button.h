#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace UI {

class Button {
public:
    using Callback = std::function<void()>;
    
public:
    Button();
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text);
    
    // Setup
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    void setText(const std::string& text);
    void setFont(const sf::Font& font);
    void setCallback(Callback callback);
    
    // Appearance
    void setNormalColor(const sf::Color& color);
    void setHoverColor(const sf::Color& color);
    void setPressedColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setCharacterSize(unsigned int size);
    
    // State
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
    // Input handling
    bool handleEvent(const sf::Event& event);
    void update(sf::Time dt);
    
    // Rendering
    void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const;
    
    // Utility
    bool contains(const sf::Vector2f& point) const;
    sf::FloatRect getBounds() const;
    
    // Activation
    void activate();
    
private:
    void updateAppearance();
    void centerText();
    
private:
    sf::RectangleShape m_shape;
    sf::Text m_text;
    Callback m_callback;
    
    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_pressedColor;
    sf::Color m_textColor;
    
    bool m_selected;
    bool m_enabled;
    bool m_hovered;
    bool m_pressed;
};

} // namespace UI
