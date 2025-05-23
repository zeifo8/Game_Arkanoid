#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Paddle.h"
#include "Ball.h"
#include "Block.h"
#include "Bonus.h"

class Game {
public:
    Game(unsigned w, unsigned h);
    void run();

private:
    void handleEvents();
    void update(sf::Time dt);
    void render();
    void initBlocks();
    void resetBallOnPaddle();
    void clearAllBonuses();

    sf::RenderWindow   m_win;
    const sf::Time     m_dt;
    Paddle             m_paddle;
    Ball               m_ball;
    std::vector<Block> m_blocks;
    std::vector<Bonus> m_bonuses;

    sf::Font  m_font;
    sf::Text  m_scoreText;
    sf::Text  m_livesText;
    sf::Text  m_overText;

    int       m_score;
    int       m_lives;
    bool      m_gameOver;

    sf::Time  m_speedTimer{};
    sf::Time  m_expandTimer{};

    sf::FloatRect getExtendedBounds(const Block& blk) const;
};
