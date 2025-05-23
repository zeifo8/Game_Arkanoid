#include "Block.h"

static const sf::Color SHADE[4] = { {144,238,144}, {60,179,113}, {34,139,34}, {0,100,0} };

Block::Block(bool indestructible, int strength, bool hasBonus, bool speedUp,
    const sf::Vector2f& size, const sf::Vector2f& pos)
    : m_indestructible(indestructible)
    , m_hits(indestructible ? 1 : std::clamp(strength, 1, 4))
    , m_hasBonus(hasBonus)
    , m_speedUp(speedUp)
    , m_destroyed(false)
{
    m_shape.setSize(size);
    m_shape.setPosition(pos);
    m_shape.setFillColor(indestructible ? sf::Color(128, 128, 128) : SHADE[m_hits - 1]);
    if (speedUp && !indestructible)
    {
        m_shape.setOutlineThickness(-3.f);
        m_shape.setOutlineColor(sf::Color::Red);
    }
}

void Block::render(sf::RenderWindow& win) const
{
    if (!m_destroyed) win.draw(m_shape);
}

bool Block::hit()
{
    if (m_indestructible) return false;
    if (--m_hits == 0) { m_destroyed = true; return true; }
    m_shape.setFillColor(SHADE[m_hits - 1]);
    return false;
}

sf::FloatRect Block::getBounds() const { return m_shape.getGlobalBounds(); }
bool Block::isDestroyed()   const { return m_destroyed; }
bool Block::containsBonus() const { return m_hasBonus; }
bool Block::isSpeedUp()     const { return m_speedUp; }
