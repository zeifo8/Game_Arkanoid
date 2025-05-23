#include "Ball.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float DEG = PI / 180.f;

    sf::Vector2f dir(float deg) { return { std::cos(deg * DEG), std::sin(deg * DEG) }; }
    sf::Vector2f norm(const sf::Vector2f& v)
    {
        float l = std::hypot(v.x, v.y); return l ? sf::Vector2f{ v.x / l, v.y / l } : sf::Vector2f{ 0,-1 };
    }
}

Ball::Ball(float w, float h)
    : m_winW(w), m_winH(h), m_baseSpeed(300.f), m_speed(m_baseSpeed)
    , m_attached(true), m_sticky(false), m_protect(false), m_dropped(false)
{
    float R = 8.f;  m_shape.setRadius(R); m_shape.setOrigin({ R,R }); m_shape.setFillColor(sf::Color::White);
    std::srand((unsigned)std::time(nullptr));
}

void Ball::attachToPaddle(const sf::Vector2f& padC)
{
    m_attached = true; m_dropped = false; m_vel = { 0,0 }; m_shape.setPosition({ padC.x, padC.y - m_shape.getRadius() - 1.f });
}

void Ball::launchRandom()
{
    if (!m_attached) return;
    float ang = -90.f + (std::rand() % 120 - 60);
    m_vel = dir(ang) * m_speed; m_attached = false;
}

void Ball::invertVelocityX() { m_vel.x = -m_vel.x; }
void Ball::invertVelocityY() { m_vel.y = -m_vel.y; }

void Ball::setSpeedMultiplier(float k) { if (k > 0) { m_speed = m_baseSpeed * k; m_vel = norm(m_vel) * m_speed; } }
void Ball::resetSpeed() { m_speed = m_baseSpeed; m_vel = norm(m_vel) * m_speed; }
void Ball::enableSticky() { m_sticky = true; }
void Ball::enableProtect() { m_protect = true; }

void Ball::randomizeDirection()
{
    float delta;
    do { delta = (std::rand() % 61 - 30); }
    while (std::abs(delta) < 10);
    float ang = std::atan2(m_vel.y, m_vel.x) / DEG + delta;
    m_vel = dir(ang);
    if (std::abs(m_vel.y) < 0.1736f)
        m_vel.y = (m_vel.y >= 0 ? 0.1736f : -0.1736f);
    m_vel = norm(m_vel) * m_speed;
}

void Ball::update(sf::Time dt, const sf::FloatRect& paddle)
{
    if (m_attached) return;

    m_shape.move(m_vel * dt.asSeconds());
    auto pos = m_shape.getPosition(); float R = m_shape.getRadius();

    if (pos.x - R < 0) { pos.x = R; invertVelocityX(); }
    if (pos.x + R > m_winW) { pos.x = m_winW - R; invertVelocityX(); }
    if (pos.y - R < 0) { pos.y = R; invertVelocityY(); }

    if (pos.y + R >= m_winH) {
        if (m_protect) { m_protect = false; pos.y = m_winH - R; invertVelocityY(); }
        else { m_dropped = true; m_attached = true; m_shape.setPosition(pos); return; }
    }

    if (m_vel.y > 0)
    {
        auto ballBounds = m_shape.getGlobalBounds();
        if (auto inter = ballBounds.findIntersection(paddle))
        {
            float R = m_shape.getRadius();
            sf::Vector2f pos = m_shape.getPosition();
            pos.y = paddle.position.y - R - 0.1f;

            invertVelocityY();

            float rel = (pos.x - paddle.position.x) / paddle.size.x - 0.5f;
            m_vel.x += rel * m_speed * 2.f;
            m_vel = norm(m_vel) * m_speed;

            if (m_sticky)
            {
                attachToPaddle({
                    paddle.position.x + paddle.size.x * 0.5f,
                    paddle.position.y
                    });
                m_sticky = false;
            }

            m_shape.setPosition(pos);
        }
    }

    if (std::abs(m_vel.y) < 0.01f * m_speed)
    {
        m_vel.y = (m_vel.y >= 0 ? 1.f : -1.f); m_vel = norm(m_vel) * m_speed;
    }

    m_shape.setPosition(pos);
}

void Ball::render(sf::RenderWindow& w) { w.draw(m_shape); }
sf::FloatRect Ball::getBounds() const { return m_shape.getGlobalBounds(); }

bool Ball::dropped() const { return m_dropped; }
void Ball::clearDropped() { m_dropped = false; }
bool Ball::isAttached() const { return m_attached; }

void Ball::increaseBaseSpeed(float k)
{
    if (k <= 0.f) return;
    m_baseSpeed *= k;
    m_speed = m_baseSpeed;
    if (!m_attached)
        m_vel = norm(m_vel) * m_speed;
}