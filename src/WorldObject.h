#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class WorldObject
{
public:
    enum class Type
    {
        Tree,
        Stone
    };

    WorldObject(Type type, sf::Vector2f pos);

    void draw(sf::RenderWindow& window) const;
    sf::FloatRect getBounds() const;
    Type getType() const { return m_type; }
    sf::FloatRect getCollisionBounds() const;



private:
    sf::Sprite m_sprite;
    Type m_type;

    static sf::Texture s_treeTexture;
    static sf::Texture s_stoneTexture;
    static bool s_loaded;
};
