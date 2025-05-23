#include <SFML/Graphics.hpp>

class Ball
{
public:
    Ball(float windowW, float windowH);
    void attachToPaddle(const sf::Vector2f& padPos);
    bool isAttached() const;
    void launchRandom();
    void update(sf::Time dt, const sf::FloatRect& paddleBounds);
    void render(sf::RenderWindow& win);
    sf::FloatRect getBounds() const;

    void invertVelocityY();
    void invertVelocityX();
    void setSpeedMultiplier(float k);
    void resetSpeed();
    void enableSticky();
    void enableProtect();
    void randomizeDirection();

    bool dropped() const;
    void clearDropped();

    void increaseBaseSpeed(float k);
private:
    sf::CircleShape m_shape;
    sf::Vector2f    m_vel;
    float           m_winW, m_winH;
    float           m_baseSpeed;
    float           m_speed;
    bool            m_attached;
    bool            m_sticky;
    bool            m_protect;
    bool            m_dropped;
};
