#include "Game.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <SFML/Window/Keyboard.hpp>

static sf::Font loadFont()
{
    sf::Font f;
    if (!f.openFromFile("assets/ArialRegular.ttf"))
        throw std::runtime_error("Can't load font!");
    return f;
}

Game::Game(unsigned w, unsigned h)
    : m_win(sf::VideoMode({ w, h }), "Arkanoid")
    , m_dt(sf::seconds(1.f / 144.f))
    , m_paddle(float(w), float(h))
    , m_ball(float(w), float(h))
    , m_font(loadFont())
    , m_scoreText(m_font, "Score: 0", 18)
    , m_livesText(m_font, "Lives: 3", 18)
    , m_overText(m_font, "GAME  OVER", 48)
    , m_score(0)
    , m_lives(3)
    , m_gameOver(false)
{
    m_scoreText.setPosition({ 10.f,  6.f });
    m_livesText.setPosition({ 10.f, 28.f });

    auto b = m_overText.getLocalBounds();
    m_overText.setOrigin({ b.size.x * .5f, b.size.y * .5f });
    m_overText.setPosition({ w * .5f, h * .5f });
    m_overText.setFillColor(sf::Color::Red);

    initBlocks();
    resetBallOnPaddle();
}

sf::FloatRect Game::getExtendedBounds(const Block& blk) const
{
    sf::FloatRect b = blk.getBounds();

    for (auto& other : m_blocks)
    {
        if (&other == &blk || other.isDestroyed())
            continue;

        auto ob = other.getBounds();

        if (std::abs((ob.position.x + ob.size.x) - b.position.x) < 1e-3f &&
            std::abs(ob.position.y - b.position.y) < 1e-3f)
        {
            b.position.x -= 2.f;
            b.size.x += 2.f;
        }
        if (std::abs((b.position.x + b.size.x) - ob.position.x) < 1e-3f &&
            std::abs(ob.position.y - b.position.y) < 1e-3f)
        {
            b.size.x += 2.f;
        }
        if (std::abs((ob.position.y + ob.size.y) - b.position.y) < 1e-3f &&
            std::abs(ob.position.x - b.position.x) < 1e-3f)
        {
            b.position.y -= 2.f;
            b.size.y += 2.f;
        }
        if (std::abs((b.position.y + b.size.y) - ob.position.y) < 1e-3f &&
            std::abs(ob.position.x - b.position.x) < 1e-3f)
        {
            b.size.y += 2.f;
        }
    }

    return b;
}

void Game::initBlocks()
{
    int rows = 6, cols = 10;
    float gap = 4.f;
    float bw = (m_win.getSize().x - (cols + 1) * gap) / cols;
    float bh = 20.f;

    int total = rows * cols, bonusCap = total / 2, bonusCnt = 0;

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
        {
            bool ind = std::rand() % 100 < 10;
            int  hp = ind ? 1 : (std::rand() % 4 + 1);
            bool bnd = !ind && bonusCnt < bonusCap && std::rand() % 100 < 20;
            if (bnd) ++bonusCnt;
            bool spd = !ind && std::rand() % 100 < 15;

            sf::Vector2f pos{ gap + j * (bw + gap),
                              50.f + gap + i * (bh + gap) };

            m_blocks.emplace_back(ind, hp, bnd, spd,
                sf::Vector2f{ bw, bh }, pos);
        }
}

void Game::resetBallOnPaddle()
{
    auto pb = m_paddle.getBounds();
    m_ball.attachToPaddle({ pb.position.x + pb.size.x * .5f,
                            pb.position.y });
}

void Game::clearAllBonuses()
{
    m_ball.resetSpeed();
    m_paddle.resetSize();
    m_speedTimer = {};
    m_expandTimer = {};
}

void Game::handleEvents()
{
    while (auto ev = m_win.pollEvent())
    {
        if (ev->is<sf::Event::Closed>())
        {
            m_win.close();
            return;
        }

        if (ev->is<sf::Event::KeyPressed>())
        {
            auto code = ev->getIf<sf::Event::KeyPressed>()->code;

            if (code == sf::Keyboard::Key::Escape)
            {
                m_win.close();
                return;
            }

            if (m_gameOver)
            {
                continue;
            }

            if (code == sf::Keyboard::Key::Space && m_ball.isAttached())
            {
                m_ball.launchRandom();
            }
        }
    }
}

void Game::update(sf::Time dt)
{
    if (m_gameOver) return;

    m_paddle.update(dt);

    if (m_ball.isAttached())
        m_ball.attachToPaddle({ m_paddle.getBounds().position.x + m_paddle.getBounds().size.x * .5f,
                                m_paddle.getBounds().position.y });

    m_ball.update(dt, m_paddle.getBounds());

    if (m_ball.dropped())
    {
        --m_lives;
        clearAllBonuses();
        resetBallOnPaddle();
        m_ball.clearDropped();
        if (m_lives == 0)
        {
            m_gameOver = true;
            m_overText.setString("GAME  OVER\nScore: " + std::to_string(m_score));
            auto b = m_overText.getLocalBounds();
            m_overText.setOrigin({ b.size.x * .5f, b.size.y * .5f });
        }
    }

    for (auto& blk : m_blocks)
    {
        if (blk.isDestroyed()) continue;

        auto extB = getExtendedBounds(blk);

        if (auto inter = m_ball.getBounds().findIntersection(extB))
        {
            if (inter->size.x < inter->size.y)
                m_ball.invertVelocityX();
            else
                m_ball.invertVelocityY();

            if (blk.isSpeedUp())
                m_ball.increaseBaseSpeed(1.1f);

            if (!blk.isIndestructible())
                m_score += blk.isSpeedUp() ? 15 : 10;

            if (blk.hit())
            {
                sf::Vector2f center = blk.getBounds().position + blk.getBounds().size * 0.5f;

                if (blk.isSpeedUp())
                    m_bonuses.emplace_back(BonusType::SpeedUp, center);

                if (blk.containsBonus())
                {
                    BonusType t = static_cast<BonusType>(1 + std::rand() % 4);
                    m_bonuses.emplace_back(t, center);
                }
            }

            break;
        }
    }

    for (auto it = m_bonuses.begin(); it != m_bonuses.end(); )
    {
        it->update(dt);
        if (it->getBounds().position.y > m_win.getSize().y)
        {
            it = m_bonuses.erase(it); continue;
        }

        if (m_paddle.getBounds().findIntersection(it->getBounds()))
        {
            switch (it->getType())
            {
            case BonusType::SpeedUp:  m_ball.setSpeedMultiplier(1.5f);
                m_speedTimer = sf::seconds(10.f); break;
            case BonusType::Expand:   m_paddle.expand();
                m_expandTimer = sf::seconds(10.f); break;
            case BonusType::Sticky:   m_ball.enableSticky();               break;
            case BonusType::Protect:  m_ball.enableProtect();              break;
            case BonusType::Redirect: m_ball.randomizeDirection();         break;
            }
            it = m_bonuses.erase(it);
        }
        else ++it;
    }

    if (m_speedTimer > sf::Time::Zero) { m_speedTimer -= dt; if (m_speedTimer <= sf::Time::Zero) m_ball.resetSpeed(); }
    if (m_expandTimer > sf::Time::Zero) { m_expandTimer -= dt; if (m_expandTimer <= sf::Time::Zero) m_paddle.resetSize(); }

    m_scoreText.setString("Score: " + std::to_string(m_score));
    m_livesText.setString("Lives: " + std::to_string(m_lives));
}

void Game::render()
{
    m_win.clear(sf::Color::Black);
    m_paddle.render(m_win);
    m_ball.render(m_win);
    for (auto& b : m_blocks)  b.render(m_win);
    for (auto& bn : m_bonuses) bn.render(m_win);

    m_win.draw(m_scoreText);
    m_win.draw(m_livesText);
    if (m_gameOver) m_win.draw(m_overText);

    m_win.display();
}

void Game::run()
{
    sf::Clock clk; sf::Time acc{};
    while (m_win.isOpen())
    {
        handleEvents();
        acc += clk.restart();
        while (acc > m_dt) { acc -= m_dt; handleEvents(); update(m_dt); }
        render();
    }
}