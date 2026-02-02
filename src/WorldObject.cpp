#include "WorldObject.h"

// ===== STATIKUS TEXTÚRÁK =====
sf::Texture WorldObject::s_treeTexture;
sf::Texture WorldObject::s_stoneTexture;
bool WorldObject::s_loaded = false;

WorldObject::WorldObject(Type type, sf::Vector2f pos)
{
    m_type = type;

    if (!s_loaded)
    {
        s_treeTexture.loadFromFile("resources/tree.png");
        s_stoneTexture.loadFromFile("resources/stone.png");
        s_loaded = true;
    }

    if (type == Type::Tree)
        m_sprite.setTexture(s_treeTexture);
    else
        m_sprite.setTexture(s_stoneTexture);

    m_sprite.setOrigin(16.f, 16.f);

    if (type == Type::Tree)
    {
        // 🌲 nagyobb fa
        m_sprite.setScale(4.f, 4.f);
    }
    else
    {
        // 🪨 kisebb kő
        m_sprite.setScale(2.f, 2.f);
    }

    m_sprite.setPosition(pos);
    

}

void WorldObject::draw(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}

sf::FloatRect WorldObject::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

sf::FloatRect WorldObject::getCollisionBounds() const
{
    sf::Vector2f pos = m_sprite.getPosition();

    if (m_type == Type::Stone)
    {
        // 🪨 POT A KŐN (KÖZÉPEN)
        return sf::FloatRect(
            pos.x - 8.f,   // középre igazítva
            pos.y - 8.f,   // 🔥 FELJEBB: a kőn van
            16.f,
            16.f
        );
    }

    return sf::FloatRect();
}

