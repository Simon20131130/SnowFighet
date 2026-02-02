#pragma once
#include <SFML/Graphics.hpp>

class Fireball
{
public:
    Fireball(sf::Vector2f pos, sf::Vector2f dir);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    bool isAlive() const;
    void kill();

    sf::FloatRect getBounds() const;

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;

    // range
    sf::Vector2f m_startPos;
    float m_maxRange = 900.f;

    bool m_alive = true;

private:
    static sf::Texture s_texture;
    static bool s_loaded;
};
