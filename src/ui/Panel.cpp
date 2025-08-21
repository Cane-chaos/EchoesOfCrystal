#include "ui/Panel.h"

namespace UI {

Panel::Panel() {
    m_shape.setFillColor(sf::Color(50, 50, 50, 200));
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(1.0f);
}

Panel::Panel(const sf::Vector2f& position, const sf::Vector2f& size)
    : Panel()
{
    setPosition(position);
    setSize(size);
}

void Panel::setPosition(const sf::Vector2f& position) {
    m_shape.setPosition(position);
}

void Panel::setSize(const sf::Vector2f& size) {
    m_shape.setSize(size);
}

void Panel::setFillColor(const sf::Color& color) {
    m_shape.setFillColor(color);
}

void Panel::setOutlineColor(const sf::Color& color) {
    m_shape.setOutlineColor(color);
}

void Panel::setOutlineThickness(float thickness) {
    m_shape.setOutlineThickness(thickness);
}

void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_shape, states);
}

sf::FloatRect Panel::getBounds() const {
    return m_shape.getGlobalBounds();
}

bool Panel::contains(const sf::Vector2f& point) const {
    return m_shape.getGlobalBounds().contains(point);
}

} // namespace UI
