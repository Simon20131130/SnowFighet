#pragma once
#include <SFML/Graphics.hpp>

class HealthPickup
{
public:
    HealthPickup(sf::Vector2f pos);

    void draw(sf::RenderWindow& window);
    bool isAlive() const;
    sf::FloatRect getBounds() const;
    int getHealAmount() const;
    void kill();

private:
    sf::Sprite m_sprite;
    static sf::Texture s_texture;
    bool m_alive;
    int m_heal;
};
