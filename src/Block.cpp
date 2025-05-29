#include "Block.h"
#include "Ball.h"

static const sf::Color SHADE[4] = {
    {144,238,144}, {60,179,113}, {34,139,34}, {0,100,0}
};

Block::Block(const sf::Vector2f& size, const sf::Vector2f& pos, bool hasBonus)
    : m_shape(size)
    , m_bonus(hasBonus)
{
    m_shape.setPosition(pos);
}

void Block::onCollision(GameObject& other)
{
    if (!isAlive()) return;
    if (auto* ball = dynamic_cast<Ball*>(&other))
        handleHit(*ball);
}

NormalBlock::NormalBlock(int hp, const sf::Vector2f& size, const sf::Vector2f& pos, bool hasBonus)
    : Block(size, pos, hasBonus)
    , m_hp(std::clamp(hp, 1, 4))
{
    m_shape.setFillColor(SHADE[m_hp - 1]);
}

void NormalBlock::handleHit(Ball&)
{
    if (--m_hp == 0)
        destroy();
    else
        m_shape.setFillColor(SHADE[m_hp - 1]);
}

IndestructibleBlock::IndestructibleBlock(const sf::Vector2f& size, const sf::Vector2f& pos)
    : Block(size, pos, false)
{
    m_shape.setFillColor({ 128,128,128 });
}

void IndestructibleBlock::handleHit(Ball&)
{

}

SpeedUpBlock::SpeedUpBlock(int hp, const sf::Vector2f& size, const sf::Vector2f& pos, bool hasBonus)
    : Block(size, pos, hasBonus)
    , m_hp(std::clamp(hp, 1, 4))
{
    m_shape.setFillColor(SHADE[m_hp - 1]);
    m_shape.setOutlineThickness(-3.f);
    m_shape.setOutlineColor(sf::Color::Red);
}

void SpeedUpBlock::handleHit(Ball& ball)
{
    ball.increaseBaseSpeed(1.1f);

    if (--m_hp == 0)
        destroy();
    else
        m_shape.setFillColor(SHADE[m_hp - 1]);
}
