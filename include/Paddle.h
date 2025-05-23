#include <SFML/Graphics.hpp>

class Paddle
{
public:
    Paddle(float windowWidth, float windowHeight);
    void update(sf::Time dt);
    void render(sf::RenderWindow& win);
    sf::FloatRect getBounds()  const;
    sf::Vector2f  getPosition() const;
    void expand();
    void resetSize();
private:
    void move(float dx);
    sf::RectangleShape m_shape;
    sf::Vector2f       m_defaultSize;
    float              m_speed;
    float              m_windowWidth;
};
