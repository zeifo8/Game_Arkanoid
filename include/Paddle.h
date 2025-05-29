#pragma once
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Paddle : public GameObject
{
public:
    Paddle(float winW, float winH);

    void update(sf::Time dt) override;
    void onCollision(GameObject&) override {}
    sf::FloatRect getBounds() const override { return m_shape.getGlobalBounds(); }
    sf::Vector2f center() const;

    void expand();
    void resetSize();

private:
    void draw(sf::RenderTarget& t, sf::RenderStates s) const override
    {
        t.draw(m_shape, s);
    }

    sf::RectangleShape m_shape;
    sf::Vector2f m_defSize{ 100.f,20.f };
    float m_speed{ 400.f };
    float m_winW;
};
