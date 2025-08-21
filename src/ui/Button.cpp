#include "ui/Button.h"
#include <iostream>

namespace UI {

Button::Button()
    : m_normalColor(sf::Color(100, 100, 100))
    , m_hoverColor(sf::Color(150, 150, 150))
    , m_pressedColor(sf::Color(200, 200, 200))
    , m_textColor(sf::Color::White)
    , m_selected(false)
    , m_enabled(true)
    , m_hovered(false)
    , m_pressed(false)
{
    m_shape.setFillColor(m_normalColor);
    m_text.setFillColor(m_textColor);
    m_text.setCharacterSize(16);
}

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text)
    : Button()
{
    setPosition(position);
    setSize(size);
    setText(text);
}

void Button::setPosition(const sf::Vector2f& position) {
    m_shape.setPosition(position);
    centerText();
}

void Button::setSize(const sf::Vector2f& size) {
    m_shape.setSize(size);
    centerText();
}

void Button::setText(const std::string& text) {
    m_text.setString(text);
    centerText();
}

void Button::setFont(const sf::Font& font) {
    m_text.setFont(font);
    centerText();
}

void Button::setCallback(Callback callback) {
    m_callback = callback;
}

void Button::setNormalColor(const sf::Color& color) {
    m_normalColor = color;
    updateAppearance();
}

void Button::setHoverColor(const sf::Color& color) {
    m_hoverColor = color;
    updateAppearance();
}

void Button::setPressedColor(const sf::Color& color) {
    m_pressedColor = color;
    updateAppearance();
}

void Button::setTextColor(const sf::Color& color) {
    m_textColor = color;
    m_text.setFillColor(color);
}

void Button::setCharacterSize(unsigned int size) {
    m_text.setCharacterSize(size);
    centerText();
}

void Button::setSelected(bool selected) {
    m_selected = selected;
    updateAppearance();
}

void Button::setEnabled(bool enabled) {
    m_enabled = enabled;
    updateAppearance();
}

bool Button::handleEvent(const sf::Event& event) {
    if (!m_enabled) return false;
    
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), 
                             static_cast<float>(event.mouseMove.y));
        bool wasHovered = m_hovered;
        m_hovered = contains(mousePos);
        
        if (wasHovered != m_hovered) {
            updateAppearance();
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                 static_cast<float>(event.mouseButton.y));
            if (contains(mousePos)) {
                m_pressed = true;
                updateAppearance();
                return true;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && m_pressed) {
            m_pressed = false;
            updateAppearance();
            
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), 
                                 static_cast<float>(event.mouseButton.y));
            if (contains(mousePos)) {
                activate();
                return true;
            }
        }
    }
    else if (event.type == sf::Event::KeyPressed) {
        if (m_selected && event.key.code == sf::Keyboard::Enter) {
            activate();
            return true;
        }
    }
    
    return false;
}

void Button::update(sf::Time dt) {
    (void)dt; // TODO: Animation or other updates can go here
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_shape, states);
    target.draw(m_text, states);
}

bool Button::contains(const sf::Vector2f& point) const {
    return m_shape.getGlobalBounds().contains(point);
}

sf::FloatRect Button::getBounds() const {
    return m_shape.getGlobalBounds();
}

void Button::activate() {
    if (m_enabled && m_callback) {
        m_callback();
    }
}

void Button::updateAppearance() {
    sf::Color color = m_normalColor;
    
    if (!m_enabled) {
        color = sf::Color(m_normalColor.r / 2, m_normalColor.g / 2, m_normalColor.b / 2);
    }
    else if (m_pressed) {
        color = m_pressedColor;
    }
    else if (m_hovered || m_selected) {
        color = m_hoverColor;
    }
    
    m_shape.setFillColor(color);
}

void Button::centerText() {
    sf::FloatRect textBounds = m_text.getLocalBounds();
    sf::FloatRect shapeBounds = m_shape.getLocalBounds();
    
    m_text.setPosition(
        m_shape.getPosition().x + (shapeBounds.width - textBounds.width) / 2.0f - textBounds.left,
        m_shape.getPosition().y + (shapeBounds.height - textBounds.height) / 2.0f - textBounds.top
    );
}

} // namespace UI
