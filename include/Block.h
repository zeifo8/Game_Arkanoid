#include <SFML/Graphics.hpp>
#include <algorithm>

class Block
{
public:
    Block(bool indestructible, int strength, bool hasBonus, bool speedUp,
        const sf::Vector2f& size, const sf::Vector2f& pos);
    void render(sf::RenderWindow& win) const;
    bool hit();
    sf::FloatRect getBounds() const;
    bool isDestroyed()   const;
    bool containsBonus() const;
    bool isSpeedUp()     const;
    bool isIndestructible() const { return m_indestructible; }
private:
    sf::RectangleShape m_shape;
    bool m_indestructible;
    int  m_hits;
    bool m_hasBonus;
    bool m_speedUp;
    bool m_destroyed;
};
