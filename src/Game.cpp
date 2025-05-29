#include "Game.h"
#include "Block.h"
#include "Bonus.h"
#include "Ball.h"
#include "Paddle.h"

#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

Game* Game::s_instance = nullptr;
Game& Game::instance() { return *s_instance; }

static sf::Font loadFont()
{
    sf::Font f;
    if (!f.openFromFile("assets/ArialRegular.ttf"))
        throw std::runtime_error("Can't load font!");
    return f;
}

Game::Game(unsigned w, unsigned h)
    : m_win(sf::VideoMode({ w,h }), "Arkanoid")
    , m_fixedDt(sf::seconds(1.f / 144.f))
    , m_font(loadFont())
    , m_scoreTxt(m_font, "Score: 0", 18)
    , m_livesTxt(m_font, "Lives: 3", 18)
    , m_overTxt(m_font, "", 48)
{
    s_instance = this;

    m_scoreTxt.setPosition({ 10,6 });
    m_livesTxt.setPosition({ 10,28 });
    m_overTxt.setFillColor(sf::Color::Red);

    m_pad = new Paddle(w, h);
    m_ball = new Ball(w, h);
    m_objs.emplace_back(m_pad);
    m_objs.emplace_back(m_ball);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int rows = 6, cols = 10;
    m_totalBlocks = rows * cols;
    m_bonusLimit = m_totalBlocks / 3;
    m_bonusCount = 0;

    initBlocks();
    restartBall();
}

void Game::initBlocks()
{
    const int rows = 6, cols = 10;
    const float gap = 4.f;
    float bw = (m_win.getSize().x - (cols + 1) * gap) / cols;
    float bh = 20.f;

    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
        {
            bool ind = std::rand() % 100 < 10;
            bool spd = !ind && (std::rand() % 100 < 15);
            bool hasBonus = !ind && (std::rand() % 100 < 20);
            int  hp = ind ? 1 : (std::rand() % 4 + 1);

            sf::Vector2f sz{ bw, bh };
            sf::Vector2f pos{ gap + j * (bw + gap), 50.f + gap + i * (bh + gap) };

            if (ind)
                m_objs.emplace_back(std::make_unique<IndestructibleBlock>(sz, pos));
            else if (spd)
                m_objs.emplace_back(std::make_unique<SpeedUpBlock>(hp, sz, pos, hasBonus));
            else
                m_objs.emplace_back(std::make_unique<NormalBlock>(hp, sz, pos, hasBonus));
        }
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
            auto key = ev->getIf<sf::Event::KeyPressed>()->code;
            if (key == sf::Keyboard::Key::Escape)
            {
                m_win.close();
                return;
            }

            if (!m_gameOver && key == sf::Keyboard::Key::Space &&  m_ball->stuck())
            {
                m_ball->launchRandom();
            }
        }
    }
}

static sf::FloatRect inflate(const sf::FloatRect& r, float px = 2.f)
{
    return {
        r.position - sf::Vector2f{px,px},
        r.size + sf::Vector2f{px * 2,px * 2}
    };
}

void Game::processCollisions()
{
    for (size_t i = 0; i < m_objs.size(); ++i)
        for (size_t j = i + 1; j < m_objs.size(); ++j)
        {
            auto& A = *m_objs[i];
            auto& B = *m_objs[j];
            if (!A.isAlive() || !B.isAlive()) continue;

            bool aBall = dynamic_cast<Ball*>(&A) != nullptr;
            bool bBall = dynamic_cast<Ball*>(&B) != nullptr;

            const sf::FloatRect& rA = (aBall || bBall) ? A.getBounds() : inflate(A.getBounds());
            const sf::FloatRect& rB = (aBall || bBall) ? B.getBounds() : inflate(B.getBounds());

            if (!rA.findIntersection(rB)) continue;

            Ball* ball = aBall ? static_cast<Ball*>(&A) : static_cast<Ball*>(&B);
            Block* blk = dynamic_cast<Block*>(aBall ? &B : &A);

            if (ball && blk && blk->isAlive())
            {
                if (!dynamic_cast<IndestructibleBlock*>(blk))
                {
                    if (dynamic_cast<SpeedUpBlock*>(blk))
                        m_score += 2;
                    else
                        m_score += 1;
                }
            }

            A.onCollision(B);
            B.onCollision(A);
        }
}

void Game::update(sf::Time dt)
{
    if (m_gameOver) return;

    if (m_ball->stuck())
        m_ball->attachTo(m_pad->center());

    for (auto& o : m_objs)
        if (o->isAlive()) o->update(dt);

    processCollisions();

    if (m_ball->dropped())
    {
        if (--m_lives == 0) m_gameOver = true;
        restartBall();
        m_ball->clearDropped();
    }

    for (auto& o : m_objs)
    {
        if (auto* blk = dynamic_cast<Block*>(o.get()))
        {
            if (!blk->isAlive() && !blk->bonusSpawned())
            {
                auto center = blk->getBounds().position + blk->getBounds().size * 0.5f;
                spawnBonus(center);
                blk->markBonusSpawned();
            }
        }
    }

    m_objs.erase(std::remove_if(m_objs.begin(), m_objs.end(),
        [&](auto& p) {
            if (p->isAlive()) return false;
            if (auto* bn = dynamic_cast<Bonus*>(p.get()))
                return bn->getBounds().position.y > m_win.getSize().y;
            return true;
        }), m_objs.end());

    if (m_speedTimer > sf::Time::Zero)
    {
        m_speedTimer -= dt;
        if (m_speedTimer <= sf::Time::Zero) m_ball->resetSpeed();
    }
    if (m_expandTimer > sf::Time::Zero)
    {
        m_expandTimer -= dt;
        if (m_expandTimer <= sf::Time::Zero) m_pad->resetSize();
    }

    m_scoreTxt.setString("Score: " + std::to_string(m_score));
    m_livesTxt.setString("Lives: " + std::to_string(m_lives));
    if (m_gameOver)
    {
        m_overTxt.setString("GAME OVER\nScore: " + std::to_string(m_score));
        auto b = m_overTxt.getLocalBounds();
        m_overTxt.setOrigin({ b.size.x * 0.5f, b.size.y * 0.5f });
        m_overTxt.setPosition({ m_win.getSize().x * 0.5f, m_win.getSize().y * 0.5f });
    }
}

void Game::spawnBonus(const sf::Vector2f& pos)
{
    if (m_bonusCount >= m_bonusLimit) return;
    if ((std::rand() % 100) >= 35)    return;

    switch (std::rand() % 5)
    {
    case 0: m_objs.emplace_back(std::make_unique<SpeedUpBonus >(pos)); break;
    case 1: m_objs.emplace_back(std::make_unique<ExpandBonus  >(pos)); break;
    case 2: m_objs.emplace_back(std::make_unique<StickyBonus  >(pos)); break;
    case 3: m_objs.emplace_back(std::make_unique<ProtectBonus >(pos)); break;
    case 4: m_objs.emplace_back(std::make_unique<RedirectBonus>(pos)); break;
    }
    ++m_bonusCount;
}

void Game::restartBall()
{
    m_ball->resetToDefaultSpeed();
    m_ball->attachTo(m_pad->center());
}

void Game::render()
{
    m_win.clear(sf::Color::Black);
    for (auto& o : m_objs)
        if (o->isAlive()) m_win.draw(*o);
    m_win.draw(m_scoreTxt);
    m_win.draw(m_livesTxt);
    if (m_gameOver) m_win.draw(m_overTxt);
    m_win.display();
}

void Game::run()
{
    sf::Clock clk;
    sf::Time  acc{};
    while (m_win.isOpen())
    {
        handleEvents();
        acc += clk.restart();
        while (acc > m_fixedDt)
        {
            acc -= m_fixedDt;
            update(m_fixedDt);
        }
        render();
    }
}
