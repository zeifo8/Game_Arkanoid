#include "Bonus.h"

Bonus::Bonus(BonusType type, const sf::Vector2f& pos)
    : m_type(type), m_speed(200.f)
{
    m_shape.setRadius(10.f);
    m_shape.setOrigin({ 10.f,10.f });
    m_shape.setPosition(pos);

    if (type == BonusType::SpeedUp)      m_shape.setFillColor(sf::Color::Red);
    else if (type == BonusType::Expand)  m_shape.setFillColor(sf::Color::Green);
    else if (type == BonusType::Sticky)  m_shape.setFillColor(sf::Color::Yellow);
    else if (type == BonusType::Protect) m_shape.setFillColor(sf::Color::Cyan);
    else                                 m_shape.setFillColor(sf::Color::Magenta);
}

void Bonus::update(sf::Time dt) { m_shape.move({ 0.f, m_speed * dt.asSeconds() }); }
void Bonus::render(sf::RenderWindow& win) { win.draw(m_shape); }
sf::FloatRect Bonus::getBounds() const { return m_shape.getGlobalBounds(); }
BonusType     Bonus::getType()   const { return m_type; }
