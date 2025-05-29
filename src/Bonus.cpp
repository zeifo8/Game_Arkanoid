#include "Bonus.h"
#include "Game.h"
#include "Ball.h"
#include "Paddle.h"

Bonus::Bonus(const sf::Vector2f& pos)
{
    m_shape.setRadius(10.f);
    m_shape.setOrigin({ 10,10 });
    m_shape.setPosition(pos);
}

void Bonus::update(sf::Time dt)
{
    m_shape.move({ 0, m_speed * dt.asSeconds() });
}

void Bonus::onCollision(GameObject& other)
{
    if (!m_alive) return;
    if (dynamic_cast<Paddle*>(&other))
    {
        apply(Game::instance());
        destroy();
    }
}

SpeedUpBonus::SpeedUpBonus(const sf::Vector2f& p) :Bonus(p)
{
    m_shape.setFillColor(sf::Color::Red);
}

void SpeedUpBonus::apply(Game& g)
{
    g.ball().setSpeedMultiplier(1.5f);
    g.setSpeedTimer(sf::seconds(10));
}

ExpandBonus::ExpandBonus(const sf::Vector2f& p) :Bonus(p)
{
    m_shape.setFillColor(sf::Color::Green);
}

void ExpandBonus::apply(Game& g)
{
    g.paddle().expand();
    g.setExpandTimer(sf::seconds(10));
}

StickyBonus::StickyBonus(const sf::Vector2f& p) :Bonus(p)
{
    m_shape.setFillColor(sf::Color::Yellow);
}

void StickyBonus::apply(Game& g)
{
    g.ball().enableSticky();
}

ProtectBonus::ProtectBonus(const sf::Vector2f& p) :Bonus(p)
{
    m_shape.setFillColor(sf::Color::Cyan);
}

void ProtectBonus::apply(Game& g)
{
    g.ball().enableProtect();
}

RedirectBonus::RedirectBonus(const sf::Vector2f& p) :Bonus(p)
{
    m_shape.setFillColor(sf::Color::Magenta);
}

void RedirectBonus::apply(Game& g)
{
    g.ball().randomizeDirection();
}
