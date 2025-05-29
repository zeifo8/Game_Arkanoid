#include "Ball.h"
#include "Paddle.h"
#include "Block.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <optional>

static float toRad(float deg) { return deg * 3.1415926535f / 180.f; }

Ball::Ball(float w, float h) : m_winW(w), m_winH(h)
{
    float R = 8.f;
    m_shape.setRadius(R);
    m_shape.setOrigin({ R,R });
    m_shape.setFillColor(sf::Color::White);
    std::srand(unsigned(std::time(nullptr)));
}

void Ball::attachTo(const sf::Vector2f& p)
{
    m_stuck = true;
    m_dropped = false;
    m_vel = { 0,0 };
    m_shape.setPosition({ p.x, p.y - m_shape.getRadius() - 1 });
}

void Ball::launchRandom()
{
    if (!m_stuck) return;
    float ang = -90 + (std::rand() % 120 - 60);
    m_vel = norm({ std::cos(toRad(ang)), std::sin(toRad(ang)) }) * m_speed;
    m_stuck = false;
}

sf::Vector2f Ball::norm(const sf::Vector2f& v) const
{
    float len = std::hypot(v.x, v.y);
    return len > 1e-5f ? sf::Vector2f{ v.x / len, v.y / len } : sf::Vector2f{ 0,-1 };
}

void Ball::setSpeedMultiplier(float k)
{
    if (k <= 0) return;
    m_speed = m_base * k;
    m_vel = norm(m_vel) * m_speed;
}
void Ball::increaseBaseSpeed(float k)
{
    if (k <= 0) return;
    m_base = std::min(m_base * k, 700.f);
    m_speed = m_base;
    if (!m_stuck) m_vel = norm(m_vel) * m_speed;
}
void Ball::resetSpeed()
{
    m_speed = m_base;
    m_vel = norm(m_vel) * m_speed;
}
void Ball::resetToDefaultSpeed()
{
    m_base = kDefaultBase;
    resetSpeed();
}

void Ball::enableSticky() { m_sticky = true; }
void Ball::enableProtect() { m_protect = true; }

void Ball::randomizeDirection()
{
    if (m_stuck) return;
    float ang{};
    do { ang = float(std::rand() % 360); } while (std::abs(std::sin(toRad(ang))) < 0.25f);
    m_vel = norm({ std::cos(toRad(ang)), std::sin(toRad(ang)) }) * m_speed;
}

void Ball::update(sf::Time dt)
{
    if (m_stuck) return;

    m_shape.move(m_vel * dt.asSeconds());
    auto pos = m_shape.getPosition();
    float R = m_shape.getRadius();

    if (pos.x - R < 0) { pos.x = R;           m_vel.x = -m_vel.x; }
    if (pos.x + R > m_winW) { pos.x = m_winW - R;  m_vel.x = -m_vel.x; }
    if (pos.y - R < 0) { pos.y = R;           m_vel.y = -m_vel.y; }

    if (pos.y + R >= m_winH)
    {
        if (m_protect) { m_protect = false; pos.y = m_winH - R; m_vel.y = -m_vel.y; }
        else { m_dropped = true; m_stuck = true; }
    }
    m_shape.setPosition(pos);
}

void Ball::onCollision(GameObject& other)
{
    if (!m_alive) return;

    if (auto* pad = dynamic_cast<Paddle*>(&other))
    {
        if (m_vel.y > 0)
        {
            auto pb = pad->getBounds();
            auto pos = m_shape.getPosition();
            float R = m_shape.getRadius();

            pos.y = pb.position.y - R - .1f;
            m_shape.setPosition(pos);
            m_vel.y = -m_vel.y;

            float rel = (pos.x - pb.position.x) / pb.size.x - 0.5f;
            m_vel.x += rel * m_speed * 2.f;
            m_vel = norm(m_vel) * m_speed;

            if (m_sticky) {
                attachTo({ pb.position.x + pb.size.x * 0.5f, pb.position.y });
                m_sticky = false;
            }
        }
    }
    else if (auto* blk = dynamic_cast<Block*>(&other))
    {
        std::optional<sf::FloatRect> inter = getBounds().findIntersection(blk->getBounds());
        if (!inter) return;

        auto pos = m_shape.getPosition();
        auto br = blk->getBounds();
        float R = m_shape.getRadius();

        if (inter->size.x < inter->size.y)
        {
            pos.x = pos.x < br.position.x ? br.position.x - R - 1.f : br.position.x + br.size.x + R + 1.f;
            m_vel.x = -m_vel.x;
        }
        else
        {
            pos.y = pos.y < br.position.y ? br.position.y - R - 1.f : br.position.y + br.size.y + R + 1.f;
            m_vel.y = -m_vel.y;
        }
        m_shape.setPosition(pos);
    }
}
