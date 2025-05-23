#include "Paddle.h"
#include <SFML/Window/Keyboard.hpp>

Paddle::Paddle(float w, float h)
    : m_speed(400.f), m_windowWidth(w)
{
    m_defaultSize = { 100.f,20.f };
    m_shape.setSize(m_defaultSize);
    m_shape.setFillColor(sf::Color::White);
    m_shape.setPosition({ (w - m_defaultSize.x) / 2.f, h - 50.f });
}

sf::Vector2f  Paddle::getPosition() const { return m_shape.getPosition(); }
sf::FloatRect Paddle::getBounds()   const { return m_shape.getGlobalBounds(); }

void Paddle::move(float dx)
{
    m_shape.move({ dx,0.f });
    auto p = m_shape.getPosition();
    if (p.x < 0) p.x = 0;
    if (p.x + m_shape.getSize().x > m_windowWidth)
        p.x = m_windowWidth - m_shape.getSize().x;
    m_shape.setPosition(p);
}

void Paddle::update(sf::Time dt)
{
    float d = m_speed * dt.asSeconds();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  move(-d);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) move(d);
}

void Paddle::render(sf::RenderWindow& win) { win.draw(m_shape); }

void Paddle::expand()
{
    auto s = m_shape.getSize();
    s.x = std::min(s.x * 1.5f, m_windowWidth);
    m_shape.setSize(s);
}

void Paddle::resetSize() { m_shape.setSize(m_defaultSize); }
