#pragma once
#include <SFML/Graphics.hpp>

class GameObject : public sf::Drawable
{
public:
    virtual ~GameObject() = default;

    virtual void update(sf::Time dt) = 0;
    virtual void onCollision(GameObject& other) = 0;
    virtual sf::FloatRect getBounds() const = 0;

    bool isAlive() const { return m_alive; }
    void destroy() { m_alive = false; }

protected:
    bool m_alive{ true };
};
