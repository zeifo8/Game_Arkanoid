#pragma once
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Paddle;

class Ball : public GameObject
{
public:
    Ball(float winW, float winH);

    void attachTo(const sf::Vector2f& topOfPad);
    bool stuck() const { return m_stuck; }
    void launchRandom();

    void update(sf::Time dt) override;
    void onCollision(GameObject& other) override;
    sf::FloatRect getBounds() const override { return m_shape.getGlobalBounds(); }

    void setSpeedMultiplier(float k);
    void increaseBaseSpeed(float k);
    void resetSpeed();
    void resetToDefaultSpeed();
    void enableSticky();
    void enableProtect();
    void randomizeDirection();

    bool dropped() const { return m_dropped; }
    void clearDropped() { m_dropped = false; }

private:
    void draw(sf::RenderTarget& t, sf::RenderStates s) const override
    {
        t.draw(m_shape, s);
    }

    sf::Vector2f norm(const sf::Vector2f& v) const;

    sf::CircleShape m_shape;
    sf::Vector2f m_vel;
    float m_winW, m_winH;

    static constexpr float kDefaultBase = 300.f;
    float m_base{ kDefaultBase }, m_speed{ m_base };
    bool m_stuck{ true }, m_sticky{ false },
        m_protect{ false }, m_dropped{ false };
};
