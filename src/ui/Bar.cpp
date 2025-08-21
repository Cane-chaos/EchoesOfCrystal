#include "ui/Bar.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace UI {

Bar::Bar()
    : m_currentValue(100.0f)
    , m_maximumValue(100.0f)
    , m_displayValue(100.0f)
    , m_textFormat("{current}/{max}")
    , m_showText(true)
    , m_animated(true)
    , m_animationSpeed(200.0f)
{
    m_background.setFillColor(sf::Color(50, 50, 50));
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(1.0f);
    
    m_fill.setFillColor(sf::Color::Green);
    
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::White);
    
    updateText();
    updateFillWidth();
}

Bar::Bar(const sf::Vector2f& position, const sf::Vector2f& size)
    : Bar()
{
    setPosition(position);
    setSize(size);
}

void Bar::setPosition(const sf::Vector2f& position) {
    m_background.setPosition(position);
    m_fill.setPosition(position);
    
    // Center text in the bar
    sf::FloatRect barBounds = m_background.getGlobalBounds();
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setPosition(
        position.x + (barBounds.width - textBounds.width) / 2.0f,
        position.y + (barBounds.height - textBounds.height) / 2.0f
    );
}

void Bar::setSize(const sf::Vector2f& size) {
    m_background.setSize(size);
    updateFillWidth();
    
    // Reposition text
    setPosition(m_background.getPosition());
}

void Bar::setValues(float current, float maximum) {
    m_currentValue = std::max(0.0f, current);
    m_maximumValue = std::max(1.0f, maximum);
    
    if (!m_animated) {
        m_displayValue = m_currentValue;
    }
    
    updateText();
    updateFillWidth();
}

void Bar::setCurrentValue(float current) {
    setValues(current, m_maximumValue);
}

void Bar::setMaximumValue(float maximum) {
    setValues(m_currentValue, maximum);
}

void Bar::setBackgroundColor(const sf::Color& color) {
    m_background.setFillColor(color);
}

void Bar::setFillColor(const sf::Color& color) {
    m_fill.setFillColor(color);
}

void Bar::setBorderColor(const sf::Color& color) {
    m_background.setOutlineColor(color);
}

void Bar::setBorderThickness(float thickness) {
    m_background.setOutlineThickness(thickness);
}

void Bar::setFont(const sf::Font& font) {
    m_text.setFont(font);
    setPosition(m_background.getPosition()); // Recenter text
}

void Bar::setTextColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

void Bar::setCharacterSize(unsigned int size) {
    m_text.setCharacterSize(size);
    setPosition(m_background.getPosition()); // Recenter text
}

void Bar::setShowText(bool show) {
    m_showText = show;
}

void Bar::setTextFormat(const std::string& format) {
    m_textFormat = format;
    updateText();
}

void Bar::setAnimated(bool animated) {
    m_animated = animated;
    if (!animated) {
        m_displayValue = m_currentValue;
        updateFillWidth();
    }
}

void Bar::setAnimationSpeed(float speed) {
    m_animationSpeed = speed;
}

void Bar::update(sf::Time dt) {
    if (m_animated && std::abs(m_displayValue - m_currentValue) > 0.1f) {
        float direction = (m_currentValue > m_displayValue) ? 1.0f : -1.0f;
        float change = m_animationSpeed * dt.asSeconds() * direction;
        
        m_displayValue += change;
        
        // Clamp to target
        if (direction > 0 && m_displayValue > m_currentValue) {
            m_displayValue = m_currentValue;
        } else if (direction < 0 && m_displayValue < m_currentValue) {
            m_displayValue = m_currentValue;
        }
        
        updateFillWidth();
        updateText();
    }
}

void Bar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_background, states);
    target.draw(m_fill, states);
    
    if (m_showText) {
        target.draw(m_text, states);
    }
}

float Bar::getPercentage() const {
    return (m_currentValue / m_maximumValue) * 100.0f;
}

sf::FloatRect Bar::getBounds() const {
    return m_background.getGlobalBounds();
}

void Bar::updateText() {
    if (!m_showText) return;
    
    std::string text = m_textFormat;
    
    // Replace placeholders
    size_t pos = text.find("{current}");
    if (pos != std::string::npos) {
        text.replace(pos, 9, std::to_string(static_cast<int>(m_displayValue)));
    }
    
    pos = text.find("{max}");
    if (pos != std::string::npos) {
        text.replace(pos, 5, std::to_string(static_cast<int>(m_maximumValue)));
    }
    
    pos = text.find("{percent}");
    if (pos != std::string::npos) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << getPercentage();
        text.replace(pos, 9, oss.str());
    }
    
    m_text.setString(text);
    
    // Recenter text
    setPosition(m_background.getPosition());
}

void Bar::updateFillWidth() {
    float percentage = m_displayValue / m_maximumValue;
    percentage = std::max(0.0f, std::min(1.0f, percentage));
    
    sf::Vector2f size = m_background.getSize();
    size.x *= percentage;
    m_fill.setSize(size);
}

} // namespace UI
