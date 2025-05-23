#include <SFML/Graphics.hpp>

enum class BonusType { SpeedUp, Expand, Sticky, Protect, Redirect };

class Bonus
{
public:
    Bonus(BonusType type, const sf::Vector2f& pos);
    void update(sf::Time dt);
    void render(sf::RenderWindow& win);
    sf::FloatRect getBounds() const;
    BonusType getType() const;
private:
    BonusType       m_type;
    sf::CircleShape m_shape;
    float           m_speed;
};
