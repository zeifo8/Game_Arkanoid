#include "Paddle.h"
#include <SFML/Window/Keyboard.hpp>

Paddle::Paddle(float w, float h) : m_winW(w)
{
    m_shape.setSize(m_defSize);
    m_shape.setFillColor(sf::Color::White);
    m_shape.setPosition({ (w - m_defSize.x) * 0.5f, h - 50.f });
}

void Paddle::update(sf::Time dt)
{
    float d = m_speed * dt.asSeconds();
    float dx = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dx -= d;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dx += d;

    m_shape.move({ dx,0 });
    auto p = m_shape.getPosition();
    if (p.x < 0) p.x = 0;
    if (p.x + m_shape.getSize().x > m_winW)
        p.x = m_winW - m_shape.getSize().x;
    m_shape.setPosition(p);
}

sf::Vector2f Paddle::center() const
{
    auto p = m_shape.getPosition();
    auto s = m_shape.getSize();
    return { p.x + s.x * 0.5f, p.y };
}

void Paddle::expand()
{
    auto s = m_shape.getSize();
    s.x = std::min(s.x * 1.5f, m_winW);
    m_shape.setSize(s);
}

void Paddle::resetSize() { m_shape.setSize(m_defSize); }
