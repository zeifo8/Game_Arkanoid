#pragma once
#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Game;

class Bonus : public GameObject
{
public:
    explicit Bonus(const sf::Vector2f& pos);

    void update(sf::Time dt) override;
    sf::FloatRect getBounds() const override { return m_shape.getGlobalBounds(); }
    void onCollision(GameObject& other) override;

protected:
    virtual void apply(Game& game) = 0;

    void draw(sf::RenderTarget& t, sf::RenderStates s) const override
    {
        if (m_alive) t.draw(m_shape, s);
    }

    sf::CircleShape m_shape;
    float m_speed{ 200.f };
};

class SpeedUpBonus : public Bonus { public: explicit SpeedUpBonus(const sf::Vector2f& p); private: void apply(Game&) override; };
class ExpandBonus : public Bonus { public: explicit ExpandBonus(const sf::Vector2f& p); private: void apply(Game&) override; };
class StickyBonus : public Bonus { public: explicit StickyBonus(const sf::Vector2f& p); private: void apply(Game&) override; };
class ProtectBonus : public Bonus { public: explicit ProtectBonus(const sf::Vector2f& p); private: void apply(Game&) override; };
class RedirectBonus : public Bonus { public: explicit RedirectBonus(const sf::Vector2f& p); private: void apply(Game&) override; };
