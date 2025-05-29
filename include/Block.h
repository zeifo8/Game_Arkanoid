#pragma once

#include "GameObject.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

class Ball;

class Block : public GameObject
{
public:
    Block(const sf::Vector2f& size, const sf::Vector2f& pos, bool hasBonus);

    virtual ~Block() = default;

    void update(sf::Time) override {}

    sf::FloatRect  getBounds() const override
    {
        return m_shape.getGlobalBounds();
    }

    void onCollision(GameObject& other) override;

    bool hasBonus() const { return m_bonus; }
    bool bonusSpawned() const { return m_bonusSpawned; }
    void markBonusSpawned() { m_bonusSpawned = true; }

protected:
    void draw(sf::RenderTarget& target,
        sf::RenderStates  states) const override
    {
        if (m_alive)
            target.draw(m_shape, states);
    }

    virtual void handleHit(Ball& ball) = 0;

    sf::RectangleShape m_shape;
    bool m_bonus;
    bool m_bonusSpawned{ false };
};

class NormalBlock : public Block
{
public:
    NormalBlock(int hp, const sf::Vector2f& size, const sf::Vector2f& pos, bool hasBonus);

protected:
    void handleHit(Ball& ball) override;

private:
    int m_hp;
};

class IndestructibleBlock : public Block
{
public:
    IndestructibleBlock(const sf::Vector2f& size, const sf::Vector2f& pos);

protected:
    void handleHit(Ball& ball) override;
};

class SpeedUpBlock : public Block
{
public:
    SpeedUpBlock(int hp, const sf::Vector2f& size, const sf::Vector2f& pos, bool hasBonus);

protected:
    void handleHit(Ball& ball) override;

private:
    int m_hp;
};
