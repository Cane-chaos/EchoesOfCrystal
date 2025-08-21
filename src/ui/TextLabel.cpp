#include "ui/TextLabel.h"

namespace UI {

TextLabel::TextLabel()
    : m_alignment(Alignment::Left)
    , m_basePosition(0, 0)
{
    m_text.setCharacterSize(16);
    m_text.setFillColor(sf::Color::White);
}

TextLabel::TextLabel(const sf::Vector2f& position, const std::string& text)
    : TextLabel()
{
    setPosition(position);
    setText(text);
}

void TextLabel::setPosition(const sf::Vector2f& position) {
    m_basePosition = position;
    m_text.setPosition(position);
    updateAlignment();
}

void TextLabel::setText(const std::string& text) {
    m_text.setString(text);
    updateAlignment();
}

void TextLabel::setFont(const sf::Font& font) {
    m_text.setFont(font);
    updateAlignment();
}

void TextLabel::setCharacterSize(unsigned int size) {
    m_text.setCharacterSize(size);
    updateAlignment();
}

void TextLabel::setFillColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

void TextLabel::setOutlineColor(const sf::Color& color) {
    m_text.setOutlineColor(color);
}

void TextLabel::setOutlineThickness(float thickness) {
    m_text.setOutlineThickness(thickness);
}

void TextLabel::setStyle(sf::Uint32 style) {
    m_text.setStyle(style);
}

void TextLabel::setAlignment(Alignment alignment) {
    m_alignment = alignment;
    updateAlignment();
}

void TextLabel::setOrigin(const sf::Vector2f& origin) {
    m_text.setOrigin(origin);
}

void TextLabel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_text, states);
}

sf::FloatRect TextLabel::getBounds() const {
    return m_text.getGlobalBounds();
}

sf::Vector2f TextLabel::getPosition() const {
    return m_text.getPosition();
}

std::string TextLabel::getString() const {
    return m_text.getString().toAnsiString();
}

const sf::String& TextLabel::getSfString() const {
    return m_text.getString();
}

void TextLabel::updateAlignment() {
    sf::FloatRect bounds = m_text.getLocalBounds();
    sf::Vector2f position = m_basePosition;
    
    switch (m_alignment) {
        case Alignment::Left:
            // Default position
            break;
        case Alignment::Center:
            position.x -= bounds.width / 2.0f;
            break;
        case Alignment::Right:
            position.x -= bounds.width;
            break;
    }
    
    m_text.setPosition(position);
}

} // namespace UI
