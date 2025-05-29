#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Block.h"
#include "Ball.h"
#include "Paddle.h"
#include "Bonus.h"

class Game
{
public:
    static Game& instance();

    Game(unsigned w, unsigned h);
    void run();

    Ball& ball() { return *m_ball; }
    Paddle& paddle() { return *m_pad; }

    void setSpeedTimer(sf::Time t) { m_speedTimer = t; }
    void setExpandTimer(sf::Time t) { m_expandTimer = t; }

private:
    void handleEvents();
    void update(sf::Time dt);
    void render();
    void initBlocks();
    void spawnBonus(const sf::Vector2f& pos);
    void restartBall();
    void processCollisions();

    sf::RenderWindow m_win;
    sf::Time m_fixedDt{ sf::seconds(1.f / 144.f) };

    std::vector<std::unique_ptr<GameObject>> m_objs;
    Ball* m_ball{ nullptr };
    Paddle* m_pad{ nullptr };

    sf::Font m_font;
    sf::Text m_scoreTxt, m_livesTxt, m_overTxt;

    int m_score{ 0 }, m_lives{ 3 };
    bool m_gameOver{ false };

    int m_totalBlocks{ 0 };
    int m_bonusLimit{ 0 };
    int m_bonusCount{ 0 };

    sf::Time m_speedTimer{}, m_expandTimer{};

    static Game* s_instance;
};
